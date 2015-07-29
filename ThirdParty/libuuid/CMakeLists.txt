project(uuid C)
set(UUID_SRC
	clear.c
	compare.c
	copy.c
	gen_uuid.c
	isnull.c
	pack.c
	parse.c
	randutils.c
	test_uuid.c
	unpack.c
	unparse.c
	uuid_time.c
	)

set(UUID_HDRS
	all-io.h
	c.h
	randutils.h
	uuid.h
	uuidP.h
	uuidd.h
)
add_definitions("-DHAVE_USLEEP")
add_library(uuid ${UUID_HDRS} ${UUID_SRC})