
namespace CGoGN
{

inline AttributeMultiVectorGen::AttributeMultiVectorGen(const std::string& strName, const std::string& strType):
		m_attName(strName), m_typeName(strType), m_toProcess(true)
{}

inline AttributeMultiVectorGen::AttributeMultiVectorGen(): m_toProcess(true)
{}

inline AttributeMultiVectorGen::~AttributeMultiVectorGen()
{}

inline void AttributeMultiVectorGen::toggleProcess()
{
	m_toProcess = true;
}

inline void AttributeMultiVectorGen::toggleNoProcess()
{
	m_toProcess = false;
}

inline bool AttributeMultiVectorGen::toProcess()
{
	return m_toProcess;
}

inline unsigned int AttributeMultiVectorGen::BlockSize()
{
	return _BLOCKSIZE_;
}






template <typename T>
AttributeMultiVector<T>::AttributeMultiVector(const std::string& strName, const std::string& strType):
AttributeMultiVectorGen(strName,strType)
{
	m_tableData.reserve(1024);
}

template <typename T>
AttributeMultiVector<T>::AttributeMultiVector()
{
	m_tableData.reserve(1024);
}

template <typename T>
AttributeMultiVector<T>::~AttributeMultiVector()
{
	for (typename std::vector< T* >::iterator it = m_tableData.begin(); it != m_tableData.end(); ++it)
		delete[] (*it);
}

template <typename T>
AttributeMultiVectorGen* AttributeMultiVector<T>::new_obj()
{
	return new AttributeMultiVector<T>;
}

template <typename T>
bool AttributeMultiVector<T>::copy(const AttributeMultiVectorGen* atmvg)
{
	const AttributeMultiVector<T>* atmv = dynamic_cast<const AttributeMultiVector<T>*>(atmvg);

	if (atmv == NULL)
	{
		std::cerr<< "copy attribute of different type"<<std::endl;
		return false;
	}

	for (unsigned int i = 0; i < atmv->m_tableData.size(); ++i)
	{
		std::memcpy(m_tableData[i], atmv->m_tableData[i], _BLOCKSIZE_*sizeof(T));
	}
	return true;
}

template <typename T>
bool AttributeMultiVector<T>::swap(AttributeMultiVectorGen* atmvg)
{
	AttributeMultiVector<T>* atmv = dynamic_cast<AttributeMultiVector<T>*>(atmvg);

	if (atmv == NULL)
	{
		std::cerr<< "swap attribute of different type"<<std::endl;
		return false;
	}

	m_tableData.swap(atmv->m_tableData) ;
	return true;
}

template <typename T>
bool AttributeMultiVector<T>::swap(AttributeMultiVectorGen& att)
{
	AttributeMultiVector<T>* attrib = dynamic_cast< AttributeMultiVector<T>* >(&att);
	if (attrib==NULL)
	{
		std::cerr << "Erreur, swap de container de types differents"<<std::endl;
		return false;
	}
	if (attrib->m_typeName != m_typeName)
	{
		std::cerr << "Erreur, swap de container avec noms de type differents"<<std::endl;
		return false;
	}

	m_tableData.swap(attrib->m_tableData);
	m_typeName.swap(attrib->m_typeName);

	return true;
}

template <typename T>
bool AttributeMultiVector<T>::merge(const AttributeMultiVectorGen& att)
{
	const AttributeMultiVector<T>* attrib = dynamic_cast< const AttributeMultiVector<T>* >(&att);
	if (attrib==NULL)
	{
		std::cerr << "Erreur, merge de container de types differents"<<std::endl;
		return false;
	}
	if (attrib->m_typeName != m_typeName)
	{
		std::cerr << "Erreur, merge de container avec noms de type differents"<<std::endl;
		return false;
	}

	for (typename std::vector< T* >::const_iterator it = attrib->m_tableData.begin(); it!= attrib->m_tableData.end(); ++it )
		m_tableData.push_back(*it);

	return true;
}

template <typename T>
T& AttributeMultiVector<T>::operator[](unsigned int i) { return m_tableData[i/_BLOCKSIZE_][i%_BLOCKSIZE_];}

template <typename T>
const T& AttributeMultiVector<T>::operator[](unsigned int i) const { return m_tableData[i/_BLOCKSIZE_][i%_BLOCKSIZE_];}

template <typename T>
std::string AttributeMultiVector<T>::output(unsigned int i)
{
	std::stringstream ss;
	ss << m_tableData[i/_BLOCKSIZE_][i%_BLOCKSIZE_];
	return ss.str();
}

template <typename T>
void AttributeMultiVector<T>::input(unsigned int i,const std::string& st)
{
	std::stringstream ss(st);
	ss >> m_tableData[i/_BLOCKSIZE_][i%_BLOCKSIZE_];
}

template <typename T>
void AttributeMultiVector<T>::addBlock()
{
	T* ptr = new T[_BLOCKSIZE_];
	m_tableData.push_back( ptr );
	// init
//	T* endPtr = ptr + _BLOCKSIZE_;
//	while (ptr != endPtr)
//		*ptr++ = T(0);
}

template <typename T>
void AttributeMultiVector<T>::initElt(unsigned int id)
{
	m_tableData[id/_BLOCKSIZE_][id%_BLOCKSIZE_] = T(0);
}

template <typename T>
void AttributeMultiVector<T>::copyElt(unsigned int dst, unsigned int src)
{
	m_tableData[dst/_BLOCKSIZE_][dst%_BLOCKSIZE_] = m_tableData[src/_BLOCKSIZE_][src%_BLOCKSIZE_];
}

template <typename T>
void AttributeMultiVector<T>::setNbBlocks(unsigned int nbb)
{
	if (nbb >=m_tableData.size())
	{
		for (unsigned int i= m_tableData.size(); i <nbb; ++i)
		{
//			T* ptr = new T[_BLOCKSIZE_];
//			m_tableData.push_back( ptr );
			addBlock();
		}
	}
	else
	{
		for (unsigned int i= nbb; i <m_tableData.size(); ++i)
		{
			delete[] m_tableData[i];
		}
		m_tableData.resize(nbb);
	}
}

template <typename T>
void AttributeMultiVector<T>::addBlocksBefore(unsigned int nbb)
{
	std::vector< T* > tempo;
	tempo.reserve(1024);

	for (unsigned int i= 0; i <nbb; ++i)
	{
		T* ptr = new T[_BLOCKSIZE_];
		tempo.push_back( ptr );
	}

	for (typename std::vector<T*>::iterator it=m_tableData.begin(); it!=m_tableData.end();++it)
	{
		tempo.push_back(*it);
	}
	m_tableData.swap(tempo);
}

template <typename T>
void AttributeMultiVector<T>::overwrite(unsigned int src_b, unsigned int src_id, unsigned int dst_b, unsigned int dst_id)
{
	m_tableData[dst_b][dst_id] = m_tableData[src_b][src_id];
}

template <typename T>
void AttributeMultiVector<T>::free()
{
	for (typename std::vector< T* >::iterator it = m_tableData.begin(); it != m_tableData.end(); ++it)
		delete[] (*it);
	m_tableData.clear();
}

template <typename T>
unsigned int AttributeMultiVector<T>::getStartAddresses(std::vector<void*>& addr, unsigned int& byteTableSize)
{
	byteTableSize = _BLOCKSIZE_ * sizeof(T);

	addr.reserve(m_tableData.size());
	addr.clear();

	for (typename std::vector< T* >::iterator it = m_tableData.begin(); it != m_tableData.end(); ++it)
	{
		addr.push_back(*it);
	}
	return addr.size();
}

template <typename T>
void AttributeMultiVector<T>::saveBin(CGoGNostream& fs, unsigned int id)
{
	unsigned int nbs[3];
	nbs[0] = id;
	int len1 = m_attName.size()+1;
	int len2 = m_typeName.size()+1;
	nbs[1] = len1;
	nbs[2] = len2;
	fs.write(reinterpret_cast<const char*>(nbs),3*sizeof(unsigned int));
	// store names
	char buffer[256];
	const char* s1 = m_attName.c_str();
	memcpy(buffer,s1,len1);
	const char* s2 = m_typeName.c_str();
	memcpy(buffer+len1,s2,len2);
	fs.write(reinterpret_cast<const char*>(buffer),(len1+len2)*sizeof(char));


	nbs[0] = m_tableData.size();
	nbs[1] = nbs[0] * _BLOCKSIZE_* sizeof(T);
	fs.write(reinterpret_cast<const char*>(nbs),2*sizeof(unsigned int));

	// store data blocks
	for(unsigned int i=0; i<nbs[0]; ++i)
	{
		fs.write(reinterpret_cast<const char*>(m_tableData[i]),_BLOCKSIZE_*sizeof(T));
	}
}

inline unsigned int AttributeMultiVectorGen::loadBinInfos(CGoGNistream& fs, std::string& name, std::string& type)
{
	unsigned int nbs[3];
	fs.read(reinterpret_cast<char*>(nbs), 3*sizeof(unsigned int));

	unsigned int id = nbs[0];
	unsigned int len1 = nbs[1];
	unsigned int len2 = nbs[2];

	char buffer[256];
	fs.read(buffer, (len1+len2)*sizeof(char));

	name = std::string(buffer);
	type = std::string(buffer+len1);

	return id;
}

template <typename T>
bool AttributeMultiVector<T>::loadBin(CGoGNistream& fs)
{
	unsigned int nbs[2];
	fs.read(reinterpret_cast<char*>(nbs), 2*sizeof(unsigned int));

	unsigned int nb = nbs[0];

	// load data blocks
	m_tableData.resize(nb);
	for(unsigned int i=0; i<nb; ++i)
	{
		T* ptr = new T[_BLOCKSIZE_];
		fs.read(reinterpret_cast<char*>(ptr),_BLOCKSIZE_*sizeof(T));
		m_tableData[i] = ptr;
	}

	return true;
}

inline bool AttributeMultiVectorGen::skipLoadBin(CGoGNistream& fs)
{
	unsigned int nbs[2];
	fs.read(reinterpret_cast<char*>(nbs), 2*sizeof(unsigned int));

	// get number of byte to skip
	unsigned int nbb = nbs[1];

	// check if nbb ok
	if (nbb%_BLOCKSIZE_ != 0)
	{
		std::cerr << "Error skipping wrong number of byte in attributes reading"<< std::endl;
		return false;
	}

	// skip data (no seek because of pb with gzstream)
	char* ptr = new char[_BLOCKSIZE_];
	while (nbb!=0)
	{
		nbb -= _BLOCKSIZE_;
		fs.read(reinterpret_cast<char*>(ptr),_BLOCKSIZE_);
	}
	delete[] ptr;

	return true;
}

template <typename T>
void AttributeMultiVector<T>::affect(unsigned int i, unsigned int j)
{
	if (m_toProcess)
		m_tableData[i/_BLOCKSIZE_][i%_BLOCKSIZE_] = m_tableData[j/_BLOCKSIZE_][j%_BLOCKSIZE_];
}

template <typename T>
void AttributeMultiVector<T>::add(unsigned int i, unsigned int j)
{
	if (m_toProcess)
		m_tableData[i/_BLOCKSIZE_][i%_BLOCKSIZE_] += m_tableData[j/_BLOCKSIZE_][j%_BLOCKSIZE_];
}

template <typename T>
void AttributeMultiVector<T>::sub(unsigned int i, unsigned int j)
{
	if (m_toProcess)
		m_tableData[i/_BLOCKSIZE_][i%_BLOCKSIZE_] -= m_tableData[j/_BLOCKSIZE_][j%_BLOCKSIZE_];
}

template <typename T>
void AttributeMultiVector<T>::mult(unsigned int i, double alpha)
{
	if (m_toProcess)
		m_tableData[i/_BLOCKSIZE_][i%_BLOCKSIZE_] *= alpha;
}

template <typename T>
void AttributeMultiVector<T>::div(unsigned int i, double alpha)
{
	if (m_toProcess)
		m_tableData[i/_BLOCKSIZE_][i%_BLOCKSIZE_] /= alpha;
}

template <typename T>
void AttributeMultiVector<T>::lerp(unsigned res, unsigned int i, unsigned int j, double alpha)
{
	if (m_toProcess)
	{
		T v1 = m_tableData[i/_BLOCKSIZE_][i%_BLOCKSIZE_];
		T v2 = m_tableData[j/_BLOCKSIZE_][j%_BLOCKSIZE_];

		v1 *= alpha;
		v2 *= 1.0-alpha;
		v1+=v2;
		m_tableData[res/_BLOCKSIZE_][res%_BLOCKSIZE_] = v1;

		std::cout << "LERP "<< m_tableData[i/_BLOCKSIZE_][i%_BLOCKSIZE_]<< " & " << m_tableData[j/_BLOCKSIZE_][j%_BLOCKSIZE_];
		std::cout << " = "<< m_tableData[res/_BLOCKSIZE_][res%_BLOCKSIZE_]<< std::endl;
	}
}

} // namespace CGoGN
