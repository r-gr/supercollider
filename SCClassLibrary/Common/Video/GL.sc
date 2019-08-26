/* Creates a new OpenGL context and window. */
GLWindow {
	*new { arg windowID=0, width=640, height=480, server;
		server = server ? Server.default;
		^server.listSendMsg(["/gl_newWindow", windowID, width, height])
	}

	*free { arg windowID=0, server;
		server = server ? Server.default;
		^server.listSendMsg(["/gl_freeWindow", windowID])
	}
}

GLVideo {
	*new { arg vidID, filepath, targetWindow=0, server;
		server = server ? Server.default;
		^server.listSendMsg(["/gl_v_new", vidID, filepath, targetWindow])
	}

	*read { arg vidID, filepath, targetWindow=0, server;
		server = server ? Server.default;
		^server.listSendMsg(["/gl_v_read", vidID, filepath, targetWindow])
	}

	*free { arg vidID, targetWindow=0, server;
		server = server ? Server.default;
		^server.listSendMsg(["/gl_v_free", vidID, targetWindow])
	}
}

GLImage {
	*new { arg imgID, filepath, targetWindow=0, server;
		server = server ? Server.default;
		^server.listSendMsg(["/gl_i_new", imgID, filepath, targetWindow])
	}

	*free { arg imgID, targetWindow=0, server;
		server = server ? Server.default;
		^server.listSendMsg(["/gl_i_free", imgID, targetWindow])
	}
}
