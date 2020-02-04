/**
requires filesystem abstraction I wrote.

Loading and saving functions for arbitrary objects.  Based on a model where 
a list of objects is saved within a folder.  The functions declared below require
explicit template instantiation for use.

	Example declaration of a compatible type:

	struct type_t
	{
		static constexpr const char* const EXTENSION{".dat"};

		static boost::filesystem::path& folder() const;

		static utility::ID_T load_id(std::istream&);
		static type          basic(std::istream&);

		utility::ID_T id;
	};

	Those are the minumum required member variables.

Storage model:
	The way this works is that each object is saved into a file under a folder.  So,
	the objects can be represented as both files or data structures.  Given an
	extension for the file, we can load all objects into memory.  This can be 
	a very tedious operation for the CPU to perform, however, if the objects 
	require a lot of space, so basic information loading is provided given
	the object has the member function to satisfy the requirements.  In this
	way, you can load only necessary information for menus and such, instead
	of loading entire objects.  Each object is associated with its corresponding file
	by it's ID.  In this way, unique filenames can be created and we can ensure
	the correct file is deleted for it's object without ambiguity.

Explanation of member variables/functions:

	-  static boost::filesystem::path folder() const;
			This will be the folder under which all object files of that type will be stored.
			Essentially, you must specify the folder you want to store this object in, 
			and it's better kept as a constant global, because it shouldn't change.  Const-ness isn't
			required, however, and you may make this variable non-const in order to change it on-the-fly.

	-  static constexpr const char* const EXTENSION{".dat"};
			This is the extension that will be used for all files storing
			data of the type.

	-  static utility::ID_T load_id(std::istream&);
			This member function should be provided by you, and
			should efficiently load ONLY the ID of the object.
			This is so that we can efficiently load all the ids
			found within a folder for assignment of new ids or
			reading a file's ID for removal or loading.

	-  utility::ID_T id;
			Specifies the unique ID assigned to the object.  This
			is used to associate the object with its file, and isn't
			assigned until the object is saved.

	-  static type basic(std::istream&);
			Privide this function in order to use the load_basic function.
			This should load the ID and other basic information you want to use.
			Rationale:  You may not want to load every object in its entirety for
			the purposes of menu displays, etc...  In these cases, it
			is better to load only necessary information, and then what you can do
			is just load the whole object when needed using its ID.  Use this method of
			loading if you think memory will be a problem.  Otherwise, you can just load
			everything.
*/

#ifndef UTILITY_FILE_LOADER_HPP_INCLUDED
#define UTILITY_FILE_LOADER_HPP_INCLUDED
#include <boost/filesystem.hpp>
#include <set>
#include <vector>

namespace utility
{
	using ID_T = int_least16_t;
	
	template<typename type> void              save(type&, const boost::filesystem::path& = type::folder());
	template<typename type> std::set<ID_T>    ids(const boost::filesystem::path& = type::folder());
	template<typename type> std::vector<type> load_all(const boost::filesystem::path& = type::folder());
	template<typename type> std::vector<type> load_basic(const boost::filesystem::path& = type::folder());
	template<typename type> type              load(const ID_T&, const boost::filesystem::path& = type::folder());
	template<typename type> void              remove(const ID_T&, const boost::filesystem::path& = type::folder());

}

#endif