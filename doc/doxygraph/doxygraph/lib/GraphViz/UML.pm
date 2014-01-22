package GraphViz::UML;

use strict;
use warnings;
use namespace::autoclean;
use Hash::FieldHash qw(:all);
use GraphViz2;

{ fieldhash my %hash, 'classes_by_name' }
{ fieldhash my %hash, 'classes_by_refid' }

=head1 NAME

GraphViz::UML - build GraphViz graphs representing UML class diagrams

=head1 SYNOPSIS

 use GraphViz::UML;
 my $uml = GraphViz::UML->new;
 $uml->add_class('Base', 'class_base_0');
 $uml->add_class('Derived', 'class_derived_0');
 $uml->add_generalization('Derived', 'class_base_0');
 my $gv = $uml->export;

=head1 DESCRIPTION

This module provides methods to build up a UML class diagram and export it as
a L<GraphViz2> object. It is designed to be used as a handler for
L<Doxygen::XML> but it can also be used on its own.

=head1 EXAMPLES

See L<Doxygen::XML> to see an example of how to use these two modules together.

=head1 METHODS

=over

=item new

The constructor takes no arguments.

=item export

Returns a L<GraphViz2> object that reflects the UML class diagram.

=item add_class($classname, $classid)

Adds a class to the diagram, with a unique identifier.

=item set_stereotype($classname, $stereotype)

Sets the stereotype of the named class (e.g. "abstract" or "interface").

=item add_operation($classname, $hashref)

Adds an operation to the named class. The hashref has the following keys:

=over

=item name

The operation's name.

=item type

The operation's return type.

=item properties

An arrayref of property strings (e.g. "public", "private", "const", "volatile").

=item parameters

An arrayref of arrayrefs, each one being a C<[name, type]> pair.

=item paramstring

A flat string representation of the parameters list, used for sorting.

=back

=item add_attribute($classname, $hashref)

Adds an attribute to the named class. The hashref has the following keys:

=over

=item name

The attribute's name.

=item type

The attribute's type.

=item properties

An arrayref of property strings (e.g. "public", "private", "const", "volatile").

=back

=item add_generalization($derived_name, $base_id)

Creates a generalization relationship between two classes.

=item add_aggregation($aggregator_name, $aggregee_id)

Creates an aggregation relationship between two classes.

=item add_composition($composer_name, $composee_id)

Creates a composition relationship between two classes.

=back

=head1 AUTHORS

Available through the git repository at L<http://code.google.com/p/doxygraph>.

=head1 COPYRIGHT AND LICENSE

Copyright 2013 Doxygraph project

This Source Code Form is subject to the terms of the Mozilla Public License,
version 2.0. If a copy of the MPL was not distributed with this file, You can
obtain one at L<http://mozilla.org/MPL/2.0/>.

=head1 SEE ALSO

L<http://www.graphviz.org/>.

=cut

sub add_class
{
  my ($self, $classname, $refid) = @_;
  
  my $class = {
    name => $classname,
    refid => $refid,
    stereotype => '',
    operations => [],
    attributes => [],
    generalizations => {},
    aggregations => {},
    compositions => {}
  };
  $self->classes_by_name->{$classname} = $class;
  $self->classes_by_refid->{$refid} = $class;
}

sub set_stereotype
{
  my ($self, $classname, $stereotype) = @_;
  exists $self->classes_by_name->{$classname} or die;
  
  $self->classes_by_name->{$classname}{stereotype} = $stereotype;
}

sub add_operation
{
  my ($self, $classname, $operation) = @_;
  exists $self->classes_by_name->{$classname} or die;
  
  push @{ $self->classes_by_name->{$classname}{operations} }, $operation;
}

sub add_attribute
{
  my ($self, $classname, $attribute) = @_;
  exists $self->classes_by_name->{$classname} or die;
  
  push @{ $self->classes_by_name->{$classname}{attributes} }, $attribute;
}

sub add_generalization
{
  my ($self, $classname, $baseclassid) = @_;
  exists $self->classes_by_name->{$classname} or die;
  
  exists $self->classes_by_refid->{$baseclassid} or return;
  $self->classes_by_name->{$classname}{generalizations}{$baseclassid} = 1;
}

sub add_aggregation
{
  my ($self, $classname, $aggregeeid) = @_;
  exists $self->classes_by_name->{$classname} or die;
  
  exists $self->classes_by_refid->{$aggregeeid} or return;
  $self->classes_by_name->{$classname}{aggregations}{$aggregeeid} = 1;
}

sub add_composition
{
  my ($self, $classname, $composeeid) = @_;
  exists $self->classes_by_name->{$classname} or die;
  
  exists $self->classes_by_refid->{$composeeid} or return;
  $self->classes_by_name->{$classname}{compositions}{$composeeid} = 1;
}

sub new
{
  my ($package) = @_;
  
  my $self = bless {} => $package;
  return from_hash($self, classes_by_name => {}, classes_by_refid => {});
}

sub export
{
  my ($self) = @_;
  
  my $gv = GraphViz2->new(
    global => {
      name => 'doxygraph',
      directed => 1,
      record_shape => 'record'
    }, graph => {
      rankdir => 'RL'
    }
  );
  
  my @classes = sort { $a->{name} cmp $b->{name} }
    values %{$self->classes_by_name};
  
  for my $class (@classes) {
    $gv->add_node(
      name => $class->{refid},
      label => $self->_make_label($class),
      shape => 'record'
    );
    
    for (keys %{$class->{generalizations}}) {
      $gv->add_edge(from => $class->{refid}, to => $_,
        arrowhead => 'empty', style =>
          $self->classes_by_refid->{$_}{stereotype} =~ m(abstract|interface)
          ? 'bold,dashed' : 'bold');
    }
    for (keys %{$class->{aggregations}}) {
      $gv->add_edge(from => $class->{refid}, to => $_,
        arrowtail => 'odiamond', dir => 'back');
    }
    for (keys %{$class->{compositions}}) {
      $gv->add_edge(from => $class->{refid}, to => $_,
        arrowtail => 'diamond', dir => 'back');
    }
  }
  
  return $gv;
}

# Compose a label for a GraphViz record node representing a UML class.
#
sub _make_label
{
  my ($self, $class) = @_;
  
  my $label = _q($class->{name});
  $label .= "\\n" . _angleq(_q($class->{stereotype})) if $class->{stereotype};
  $label .= "\\n|";
  $label .= _q($self->_make_operation($_)) . "\\l"
    for sort { _cmp_members($a, $b) } @{$class->{operations}};
  $label .= "|";
  $label .= _q($self->_make_attribute($_)) . "\\l"
    for sort { _cmp_members($a, $b) } @{$class->{attributes}};
  return $label;
}

# Compose a UML operation string from a member hash.
#
sub _make_operation
{
  my ($self, $operation) = @_;
  
  my $line = '';
  $line .= _make_prefix($operation);
  $line .= $operation->{name};
  $line .= " ( ";
  $line .= join ", ",
        map { $_->[0] . " : " . $_->[1] } @{$operation->{parameters}};
  $line = _clamp($line);
  $line .= " )";
  $line .= " : " . $operation->{type} if $operation->{type};
  $line .= " $_" for _make_suffix($operation);
  return $line;
}

# Compose a UML attribute string from a member hash.
#
sub _make_attribute
{
  my ($self, $attribute) = @_;
  
  my $line = '';
  $line .= _make_prefix($attribute);
  $line .= $attribute->{name};
  $line = _clamp($line);
  $line .= " : " . $attribute->{type} if $attribute->{type};
  $line .= " $_" for _make_suffix($attribute);
  return $line;
}

# Construct a UML visibility prefix from a member hash's properties array.
#
sub _make_prefix
{
  my ($member) = @_;
  
  my %props = map { $_ => 1 } @{$member->{properties}};
  my $prefix = '';
  $prefix .= '/' if $props{derived};
  $prefix .= '-' if $props{private};
  $prefix .= '+' if $props{public};
  $prefix .= '#' if $props{protected};
  return $prefix;
}

# Construct a UML properties suffix from a member hash's properties array.
#
sub _make_suffix
{
  my ($member) = @_;
  
  my %props = map { $_ => 1 } @{$member->{properties}};
  return map { "{$_}" } grep { exists $props{$_} } qw(
    readable writable initonly settable gettable final sealed add remove
    static virtual pure-virtual volatile mutable optional required package
    retain copy assign weak strong unretained);
}

# Comparison function for sorting member hashes by visibility and by name.
#
sub _cmp_members
{
  my ($a, $b) = @_;
  
  my %props = (none => 0, public => 1, protected => 2, private => 3);
  my $a_prop = _get_visibility($a);
  my $b_prop = _get_visibility($b);
  my $cmp = $props{$a_prop} <=> $props{$b_prop};
  return $cmp if $cmp;
  $cmp = $a->{name} cmp $b->{name};
  return $cmp if $cmp;
  $cmp = exists($a->{paramstring}) <=> exists($b->{paramstring});
  return $cmp if $cmp;
  return $a->{paramstring} cmp $b->{paramstring} if exists $a->{paramstring};
  return 0;
}

# Returns any visibility property present in a member hash's properties array.
#
sub _get_visibility
{
  my ($member) = @_;
  
  for my $prop (@{$member->{properties}}) {
    return $prop if grep { $_ eq $prop } qw(public private protected);
  }
  return 'none';
}

# Return the given string truncated to a maximum length and an ellipsis added.
#
sub _clamp
{
  my ($text) = @_;
  
  return (length($text) < 120) ? $text : substr($text, 0, 120) . '...';
}

# Return the given string with special GraphViz characters escaped.
#
sub _q
{
  my ($text) = @_;
  
  $text =~ s(([{}<>\|]))(\\$1)g;
  return $text;
}

# Return the given string surrounded by a pair of guillemets.
#
sub _angleq
{
  my ($text) = @_;
  
  return "\N{U+00AB}$text\N{U+00BB}";
}

1;