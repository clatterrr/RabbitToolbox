#ifndef OCTOON_IO_FSTREAM_H_
#define OCTOON_IO_FSTREAM_H_

#include <octoon/io/file_buf.h>
#include <octoon/io/iostream.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT ifstream final : public istream
		{
		public:
			ifstream() noexcept;
			ifstream(const char* path, const ios_base::open_mode mode) noexcept;
			ifstream(const wchar_t* path, const ios_base::open_mode mode) noexcept;
			ifstream(const std::string& path, const ios_base::open_mode mode) noexcept;
			ifstream(const std::wstring& path, const ios_base::open_mode mode) noexcept;
			~ifstream() noexcept;

			ifstream& open(const char* path, const ios_base::open_mode mode) noexcept;
			ifstream& open(const wchar_t* path, const ios_base::open_mode mode) noexcept;
			ifstream& open(const std::string& path, const ios_base::open_mode mode) noexcept;
			ifstream& open(const std::wstring& path, const ios_base::open_mode mode) noexcept;

			ifstream& close() noexcept;

			bool is_open() const noexcept;

		private:
			filebuf file_;
		};

		class OCTOON_EXPORT ofstream final : public ostream
		{
		public:
			ofstream() noexcept;
			ofstream(const char* path, const ios_base::open_mode mode) noexcept;
			ofstream(const wchar_t* path, const ios_base::open_mode mode) noexcept;
			ofstream(const std::string& path, const ios_base::open_mode mode) noexcept;
			ofstream(const std::wstring& path, const ios_base::open_mode mode) noexcept;
			~ofstream() noexcept;

			ofstream& open(const char* path, const ios_base::open_mode mode) noexcept;
			ofstream& open(const wchar_t* path, const ios_base::open_mode mode) noexcept;
			ofstream& open(const std::string& path, const ios_base::open_mode mode) noexcept;
			ofstream& open(const std::wstring& path, const ios_base::open_mode mode) noexcept;

			ofstream& close() noexcept;

			bool is_open() const noexcept;

		private:
			filebuf file_;
		};

		class OCTOON_EXPORT fstream final : public iostream
		{
		public:
			fstream() noexcept;
			fstream(const char* path, const ios_base::open_mode mode) noexcept;
			fstream(const wchar_t* path, const ios_base::open_mode mode) noexcept;
			fstream(const std::string& path, const ios_base::open_mode mode) noexcept;
			fstream(const std::wstring& path, const ios_base::open_mode mode) noexcept;
			~fstream() noexcept;

			fstream& open(const char* path, const ios_base::open_mode mode) noexcept;
			fstream& open(const wchar_t* path, const ios_base::open_mode mode) noexcept;
			fstream& open(const std::string& path, const ios_base::open_mode mode) noexcept;
			fstream& open(const std::wstring& path, const ios_base::open_mode mode) noexcept;

			fstream& close() noexcept;

			bool is_open() const noexcept;

		private:
			filebuf file_;
		};
	}
}

#endif
