//  dependency graph generator
//  Copyright (C) 2010 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#ifndef SERVER_DEPENDENCY_GRAPH_GENERATOR_HPP
#define SERVER_DEPENDENCY_GRAPH_GENERATOR_HPP

#include "node_graph.hpp"

namespace nova
{

class dependency_graph_generator
{
    typedef node_graph::dsp_thread_queue dsp_thread_queue;
    typedef thread_queue_item::successor_list successor_container;
    typedef std::vector<server_node*, rt_pool_allocator<abstract_synth*> > sequential_child_list;

public:
    dsp_thread_queue * operator()(node_graph * graph)
    {
        /* pessimize: reserve enough memory for the worst case */
        q = new node_graph::dsp_thread_queue(graph->synth_count_);

        fill_queue(*graph->root_group());
        return q;
    }

private:
    dsp_thread_queue * q;

    void fill_queue(group & root_group)
    {
        if (root_group.has_synth_children())
        {
            fill_queue_recursive(root_group, successor_container(0), 0);
        }
    }

    template <typename reverse_iterator>
    static inline int get_previous_activation_count(reverse_iterator it, reverse_iterator end, int previous_activation_limit)
    {
        reverse_iterator prev = it;

        for(;;)
        {
            ++prev;
            if (prev == end)
                return previous_activation_limit; // we are the first item, so we use the previous activiation limit

            server_node & node = *prev;
            if (node.is_synth())
                return 1;
            else {
                abstract_group & grp = static_cast<abstract_group&>(node);
                int tail_nodes = grp.tail_nodes();

                if (tail_nodes != 0) /* use tail nodes of previous group */
                    return tail_nodes;
                else                /* skip empty groups */
                    continue;
            }
        }
    }

    successor_container fill_queue_recursive(abstract_group & grp, successor_container const & successors, size_t activation_limit)
    {
        if (grp.is_parallel())
            return fill_queue_recursive(static_cast<parallel_group&>(grp), successors, activation_limit);
        else
            return fill_queue_recursive(static_cast<group&>(grp), successors, activation_limit);
    }

    successor_container fill_queue_recursive(group & g, successor_container const & successors_from_parent, size_t previous_activation_limit)
    {
        assert (g.has_synth_children());

        typedef server_node_list::reverse_iterator r_iterator;

        successor_container successors(successors_from_parent);

        size_t children = g.child_count();

        sequential_child_list sequential_children;
        sequential_children.reserve(g.child_synths_);

        for (r_iterator it = g.child_nodes.rbegin();
            it != g.child_nodes.rend(); ++it)
        {
            server_node & node = *it;

            if (node.is_synth()) {
                r_iterator end_of_node = it;
                --end_of_node; // one element behind the last
                std::size_t node_count = 1;

                // we fill the child nodes in reverse order to an array
                for(;;)
                {
                    sequential_children.push_back(&*it);
                    ++it;
                    if (it == g.child_nodes.rend())
                        break; // we found the beginning of this group

                    if (!it->is_synth())
                        break; // we hit a child group, later we may want to add it's nodes, too?
                    ++node_count;
                }

                --it; // we iterated one element too far, so we need to go back to the previous element
                assert(sequential_children.size() == node_count);

                sequential_child_list::reverse_iterator seq_it = sequential_children.rbegin();

                int activation_limit = get_previous_activation_count(it, g.child_nodes.rend(), previous_activation_limit);

                thread_queue_item * q_item =
                    q->allocate_queue_item(queue_node(static_cast<abstract_synth*>(*seq_it++), node_count),
                                            successors, activation_limit);

                queue_node & q_node = q_item->get_job();

                // now we can add all nodes sequentially
                for(;seq_it != sequential_children.rend(); ++seq_it)
                    q_node.add_node(static_cast<abstract_synth*>(*seq_it));
                sequential_children.clear();

                assert(q_node.size() == node_count);

                /* advance successor list */
                successors = successor_container(1);
                successors[0] = q_item;

                if (activation_limit == 0)
                    q->add_initially_runnable(q_item);
                children -= node_count;
            }
            else {
                abstract_group & grp = static_cast<abstract_group&>(node);

                if (grp.has_synth_children())
                {
                    int activation_limit = get_previous_activation_count(it, g.child_nodes.rend(), previous_activation_limit);
                    successors = fill_queue_recursive(grp, successors, activation_limit);
                }

                children -= 1;
            }
        }
        assert(children == 0);
        return successors;
    }

    successor_container fill_queue_recursive(parallel_group & g, successor_container const & successors_from_parent, size_t previous_activation_limit)
    {
        assert (g.has_synth_children());
        std::vector<thread_queue_item*, rt_pool_allocator<void*> > collected_nodes;
        collected_nodes.reserve(g.child_synths_ + g.child_groups_ * 16); // pessimize

        for (server_node_list::iterator it = g.child_nodes.begin();
            it != g.child_nodes.end(); ++it)
        {
            server_node & node = *it;

            if (node.is_synth()) {
                thread_queue_item * q_item = q->allocate_queue_item(queue_node(static_cast<abstract_synth*>(&node)),
                                                                    successors_from_parent, previous_activation_limit);

                if (previous_activation_limit == 0)
                    q->add_initially_runnable(q_item);

                collected_nodes.push_back(q_item);
            }
            else {
                abstract_group & grp = static_cast<abstract_group&>(node);

                if (grp.has_synth_children())
                {
                    successor_container group_successors = fill_queue_recursive(grp, successors_from_parent,
                                                                                previous_activation_limit);

                    for (unsigned int i = 0; i != group_successors.size(); ++i)
                        collected_nodes.push_back(group_successors[i]);
                }
            }
        }

        successor_container ret(collected_nodes.size());

        memcpy(&ret[0], &collected_nodes[0], collected_nodes.size() * sizeof(thread_queue_item *));
/*      for (std::size_t i = 0; i != collected_nodes.size(); ++i)
            ret[i] = collected_nodes[i]; */
        return ret;
    }
};

} /* namespacen nova */

#endif /* SERVER_DEPENDENCY_GRAPH_GENERATOR_HPP */
