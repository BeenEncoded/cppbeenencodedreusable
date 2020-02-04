#include <fstream>
#include <exception>
#include <stdexcept>
#include <set>
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>

#include "file_loader.hpp"
#include "filesystem.hpp"

namespace
{
	template<typename type> utility::ID_T load_id(const boost::filesystem::path&);
	template<typename type> type load_basic(const boost::filesystem::path&);
	template<typename type> type load(const boost::filesystem::path&);



	template<typename type>
	inline utility::ID_T load_id(const boost::filesystem::path& file)
	{
		utility::ID_T id;
		std::ifstream in{file.string().c_str(), std::ios::binary};
		if(in.good()) id = type::load_id(in);
		in.close();
		return id;
	}

	template<typename type>
	type load(const boost::filesystem::path& p)
	{
		type t;
		std::ifstream in{p.string().c_str(), std::ios::binary};
		if(in.good()) in>>t;
		in.close();
		return t;
	}

	template<typename type>
	type load_basic(const boost::filesystem::path& p)
	{
		type t;
		std::ifstream in{p.string().c_str(), std::ios::binary};
		if(in.good()) t = type::basic(in);
		in.close();
		return t;
	}


}

namespace utility
{
	/*
	Saves t to its file under "folder".
	*/
	template<typename type>
	void save(type& t, const boost::filesystem::path& folder)
	{
		using boost::filesystem::is_directory;
		using boost::filesystem::is_regular_file;
		using boost::filesystem::is_symlink;
		using boost::filesystem::exists;
		using boost::filesystem::create_directories;
		using ::filesystem::glob;
		using boost::filesystem::path;

		if(is_symlink(folder)) throw std::runtime_error{"Folder to save file is a symlink!"};

		//create the folder if it doesn't exist:
		if(!is_directory(folder))
		{
			create_directories(folder);
			if(!exists(folder)) throw std::runtime_error{"Unable to save!  Couldn't create folder " + folder.string()};
		}

		//assign a new id if there isn't one already:
		if(t.id == 0)
		{
			auto id{ids<type>(folder)};
			while(id.find(++t.id) != id.end());
		}
		
		//now we find its file or create it if it doesn't exist:
		boost::filesystem::path file;
		for(glob it{folder, (std::string{"**"} + type::EXTENSION + std::string{"$"}).c_str()}; (!it.end() && !exists(file)); ++it)
		{
			if(is_regular_file(*it))
			{
				if(::load_id<type>(it->path()) == t.id)
				{
					file = it->path();
				}
			}
		}

		if(!exists(file))
		{
			file = (folder / path{std::to_string(t.id) + std::string{type::EXTENSION}});
		}

		std::ofstream out{file.string().c_str(), std::ios::binary};
		out<< t;
		out.close();
	}

	/*
	Loads all ids from files under the specified folder.
	*/
	template<typename type>
	std::set<ID_T> ids(const boost::filesystem::path& folder)
	{
		using boost::filesystem::is_directory;
		using boost::filesystem::is_symlink;
		using boost::filesystem::is_regular_file;
		using ::filesystem::glob;

		std::set<ID_T> i;

		if(!is_directory(folder) || is_symlink(folder)) return i;
		for(glob it{folder, (std::string{ "**" } +type::EXTENSION + std::string{ "$" }).c_str()}; !it.end(); ++it)
		{
			if(is_regular_file(*it))
			{
				i.insert(load_id<type>(it->path()));
			}
		}
		return i;
	}

	/*
	Loads every object in its entirety.
	*/
	template<typename type>
	std::vector<type> load_all(const boost::filesystem::path& folder)
	{
		using boost::filesystem::is_directory;
		using boost::filesystem::is_symlink;
		using boost::filesystem::is_regular_file;
		using ::filesystem::glob;

		std::vector<type> t;

		if(is_directory(folder) && !is_symlink(folder))
		{
			for(glob it{folder, (std::string{"**"} + type::EXTENSION + std::string{"$"}).c_str()}; !it.end(); ++it)
			{
				if(is_regular_file(it->path()))
				{
					t.push_back(::load<type>(it->path()));
					if(t.back().id == 0) t.pop_back();
				}
			}
		}
		return t;
	}

	/*
	Deletes the file associated with the ID of object t.
	*/
	template<typename type>
	void remove(const ID_T& id, const boost::filesystem::path& folder)
	{
		using boost::filesystem::is_directory;
		using boost::filesystem::is_symlink;
		using ::filesystem::glob;
		using boost::filesystem::is_regular_file;
		using boost::filesystem::remove;
		using boost::filesystem::exists;

		if(!is_directory(folder) || is_symlink(folder)) return;
		for(glob it{folder, (std::string{"**"} + type::EXTENSION + std::string{"$"}).c_str()}; !it.end(); ++it)
		{
			if(is_regular_file(*it))
			{
				if(load_id<type>(it->path()) == id)
				{
					remove(*it);
					if(exists(*it)) throw std::runtime_error{"Error: could not remove file \"" + it->path().string() + "\""};
					return;
				}
			}
		}
	}

	/*
	Loads an object from a file given its id.
	*/
	template<typename type>
	type load(const ID_T& id, const boost::filesystem::path& folder)
	{
		using boost::filesystem::is_directory;
		using boost::filesystem::is_regular_file;
		using boost::filesystem::is_symlink;
		using ::filesystem::glob;

		if(!is_directory(folder) || is_symlink(folder)) throw std::runtime_error{"Error: unable to load from non-existant folder"};
		for(glob it{folder, (std::string{"**"} + type::EXTENSION + std::string{"$"}).c_str()}; !it.end(); ++it)
		{
			if(is_regular_file(*it))
			{
				if(load_id<type>(it->path()) == id)
				{
					return ::load<type>(it->path());
				}
			}
		}
		throw std::runtime_error{"Error: attempt to load invalid id!"};
	}

	/*
	Partially loads all objects.
	*/
	template<typename type>
	std::vector<type> load_basic(const boost::filesystem::path& folder)
	{
		using boost::filesystem::is_directory;
		using boost::filesystem::is_symlink;
		using boost::filesystem::is_regular_file;
		using ::filesystem::glob;

		std::vector<type> t;

		if(is_directory(folder) && !is_symlink(folder))
		{
			for(glob it{ folder, (std::string{ "**" } +type::EXTENSION + std::string{ "$" }).c_str() }; !it.end(); ++it)
			{
				if(is_regular_file(it->path()))
				{
					t.push_back(::load_basic<type>(it->path()));
					if(t.back().id == 0) t.pop_back();
				}
			}
		}
		return t;
	}


}

namespace utility
{
	/*template void                            save      <type>(data::account_data& t, const boost::filesystem::path& folder);
	template std::set<ID_T>                  ids       <type>(const boost::filesystem::path& folder);
	template std::vector<data::account_data> load_all  <type>(const boost::filesystem::path& folder);
	template void                            remove    <type>(const ID_T& id, const boost::filesystem::path& folder);
	template data::account_data              load      <type>(const ID_T& id, const boost::filesystem::path& folder);
	template std::vector<data::account_data> load_basic<type>(const boost::filesystem::path& folder);*/

}

