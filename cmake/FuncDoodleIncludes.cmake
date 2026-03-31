function(funcdoodle_includes)
	target_include_directories(FuncDoodle PRIVATE
		lib/glfw/include/
		${PORTAUDIO_INCLDIR}
		lib/imgui
		lib/imgui/backends
		lib/imgui_test_engine
		lib/imgui_test_engine/imgui_test_engine
		lib/glad/include
		lib/nfd/src/include
		lib/stb/
		lib/
		lib/toml++/
		src/
		test/
	)
endfunction()
