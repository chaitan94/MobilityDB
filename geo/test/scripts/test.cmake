file(GLOB geo_testfiles "geo/test/queries/*.sql")
foreach(file ${geo_testfiles})
	get_filename_component(TESTNAME ${file} NAME_WE)
	add_test(
		NAME ${TESTNAME} 
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test
		COMMAND ${PROJECT_SOURCE_DIR}/test/scripts/test.sh run_compare ${CMAKE_BINARY_DIR} ${TESTNAME} ${file} 
	)
	set_tests_properties(${TESTNAME} PROPERTIES FIXTURES_REQUIRED DB)
	set_tests_properties(${TESTNAME} PROPERTIES RESOURCE_LOCK DBLOCK)
endforeach()