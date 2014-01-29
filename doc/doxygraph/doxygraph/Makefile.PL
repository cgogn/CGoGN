use ExtUtils::MakeMaker;

WriteMakefile(
	NAME => 'doxygraph',
	AUTHOR => 'Doxygraph project http://code.google.com/p/doxygraph',
	ABSTRACT_FROM => 'doxygraph',
	VERSION_FROM => 'doxygraph',
	LICENSE => 'mozilla',
	EXE_FILES => ['doxygraph'],
	PREREQ_PM => {qw(
		File::Spec		0
		GraphViz2		0
		Hash::FieldHash		0
		lib::abs		0
		Moose			0
		namespace::autoclean	0
		XML::Rabbit		0
	)}
);