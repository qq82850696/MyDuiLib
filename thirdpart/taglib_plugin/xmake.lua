
-- add target
target("taglib_plugin")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")  	
	-- add includes directory
    add_includedirs("../taglib")
	-- links
	add_deps("taglib")
	add_links("taglib")

	-- source files
    add_files("taglib_plugin/src/*.cpp") 

	-- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("taglib_plugin/src/**.h")
    end
	
	-- add defines
    add_defines("TAGLIB_STATIC", "TAGLIB_PLUGIN_EXPORTS", "TAGLIB_PLUGIN_DLL_EXPORT", "BUILD_AS_DLL")

    add_rules("thirdpart.shared")

    set_values("vs.folder", "thirdpart/tag")