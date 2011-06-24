#ifndef FAKEATTRIBUTE_H_
#define FAKEATTRIBUTE_H_

#include <fstream>

namespace CGoGN
{
/**
 * Ajoute tout ce qu'il faut a un type (std::string par exemple)
 * qui n'a pas les operateurs += -= *= /= ..
 * pour qu'il soit utilisable (ca compile!)
 */
template <typename T>
class NoMathAttribute: public T
{
public:
	NoMathAttribute(): T() {}
	NoMathAttribute(int i): T() {}

	NoMathAttribute(const T& att): T(att) {}

	NoMathAttribute<T>& operator = (const T& fa) { *this = NoMathAttribute<T>(fa); }
	void operator += (const NoMathAttribute<T>& fa) {}
	void operator -= (const NoMathAttribute<T>& fa) {}
	void operator *= (double v) {}
	void operator /= (double v) {}
	T operator *(double v) const { return  T(); }

	static std::string CGoGNnameOfType() { return nameOfType(T()); }
};

/**
 * Ajoute tout ce qu'il faut a un type (std::string par exemple)
 * qui n'a pas les operateurs += -= *= /=
 * ni la possibilité d'ajouter une fonction CGoGNnameOfType (std::vector<> par exemple)
 * pour qu'il soit utilisable (ca compile!)
 */
template <typename T>
class NoMathNameAttribute: public T
{
public:
	NoMathNameAttribute(): T() {}
	NoMathNameAttribute(int i): T() {}

	NoMathNameAttribute(const T& att): T(att) {}

	NoMathNameAttribute<T>& operator = (const T& fa) { return *this = NoMathNameAttribute<T>(fa); }
	void operator += (const NoMathNameAttribute<T>& fa) {}
	void operator -= (const NoMathNameAttribute<T>& fa) {}
	void operator *= (double v) {}
	void operator /= (double v) {}
	T operator *(double v) const  {return  T();}

	static std::string CGoGNnameOfType() { return ""; }
};

template <typename T>
NoMathAttribute<T> operator*(T a, const NoMathAttribute<T>& v)
{
	return T();
}

/**
 * Ajoute tout ce qu'il faut a un type
 * qui n'a pas les operateurs << et >>
 * pour qu'il soit utilisable (ca compile!)
 */
template <typename T>
class NoIOAttribute: public T
{
public:
	NoIOAttribute(): T() {}
	NoIOAttribute(int i): T() {}

	NoIOAttribute(const T& att): T(att) {}
	NoIOAttribute<T>& operator = (const T& fa) { *this = NoIOAttribute<T>(fa); }

	friend std::ostream& operator<<( std::ostream &out, const NoIOAttribute<T>& fa ) { return out ; }
	friend const std::istream& operator>>( const std::istream &in, NoIOAttribute<T>& fa ) { return in ; }

	static std::string CGoGNnameOfType() { return nameOfType(T()); }
};

/**
 * Ajoute tout ce qu'il faut a un type (std::string par exemple)
 * qui n'a pas les operateurs += -= *= /=  et << , >>
 * pour qu'il soit utilisable (ca compile!)
 */
template <typename T>
class NoMathIOAttribute: public T
{
public:
	NoMathIOAttribute(): T() {}
	NoMathIOAttribute(int i): T() {}

	NoMathIOAttribute(const T& att): T(att) {}

	friend std::ostream& operator<<( std::ostream &out, const NoMathIOAttribute<T>& fa ) { return out ; }
	friend const std::istream& operator>>( const std::istream &in, NoMathIOAttribute<T>& fa ) { return in ; }

	NoMathIOAttribute<T>& operator = (const T& fa) { return *this = NoMathIOAttribute<T>(fa); }

	void operator += (const NoMathIOAttribute<T>& fa) {}
	void operator -= (const NoMathIOAttribute<T>& fa) {}
	void operator *= (double v) {}
	void operator /= (double v) {}

	static std::string CGoGNnameOfType() { return nameOfType(T());}
};

/**
 * Ajoute tout ce qu'il faut a un type (std::string par exemple)
 * qui n'a pas les operateurs += -= *= /=  et << , >>
 * ni la possibilité d'ajouter une fonction CGoGNnameOfType (std::vector<> par exemple)
 * pour qu'il soit utilisable (ca compile!)
 */
template <typename T>
class NoMathIONameAttribute: public T
{
public:
	NoMathIONameAttribute(): T() {}
	NoMathIONameAttribute(int i): T() {}

	NoMathIONameAttribute(const T& att): T(att) {}

	friend std::ostream& operator<<( std::ostream &out, const NoMathIONameAttribute<T>& fa ) { return out ; }
	friend const std::istream& operator>>( const std::istream &in, NoMathIONameAttribute<T>& fa ) { return in ; }

	NoMathIONameAttribute<T>& operator = (const T& fa) { return *this = NoMathIONameAttribute<T>(fa); }

	void operator += (const NoMathIONameAttribute<T>& fa) {}
	void operator -= (const NoMathIONameAttribute<T>& fa) {}
	void operator *= (double v) {}
	void operator /= (double v) {}

	static std::string CGoGNnameOfType() { return ""; }
};

} // namespace CGoGN

#endif /* FAKEATTRIBUTE_H_ */
