#include <functional>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <tuple>
#include <vector>
#include <array>

template <typename ... TList>
struct DefaultParsersTuple;


template <typename ... TList>
class CSVReader {
public:
	using row_type = std::tuple<TList ...>; // 1

private:
	std::vector<row_type> rows_;
	std::ifstream stream_;

	std::tuple<std::function<TList(const std::string &)>...> parsers_ =
		DefaultParsersTuple<TList ... >::get();

	char separator_ = ';';
	char quote_ = '"';
	char escape_ = '\n';
	bool ignore_bad_row_length_ = false;
	bool skip_empty_lines_ = true;
	std::size_t skip_first_lines_ = 0;

	bool first_lines_skipped_ = false;

public:
	CSVReader();
	explicit CSVReader(const char * filename);
	explicit CSVReader(const std::string & filename);

	void open(const char * filename);
	void open(const std::string & filename);

	bool is_open();
	bool is_open() const;

	char & separator();
	const char & separator() const;
	char & quote();
	const char & quote() const;
	bool & ignore_bad_row_length();
	const bool & ignore_bad_row_length() const;
	bool & skip_empty_lines();
	const bool & skip_empty_lines() const;
	std::size_t & skip_first_lines();
	const std::size_t & skip_first_lines() const;

	row_type operator[] (std::size_t i);
	const row_type operator[] (std::size_t i) const;

	std::size_t rows() const;

	std::size_t read_rows(std::size_t n = std::numeric_limits<std::size_t>::max());

	void close();
};

template <typename T>
struct DefaultParser 
{
	static T parse(const std::string & str) 
	{
		if (str.empty())
			return T();

		std::istringstream stream(str);
		T t;
		stream >> t;

		if (stream.fail())
			throw std::runtime_error("Unsupported format of '" + str + "' (fail bit set)");

		return t;
	}
};

template <>
struct DefaultParser<std::string> 
{
	static std::string parse(const std::string & str) 
	{
		return str;
	}
};

template <typename T>
struct DefaultParsersTuple<T>
{
	using parsers_type = std::tuple<std::function<T(const std::string &)>>;

	static parsers_type get() 
	{
		return parsers_type(DefaultParser<T>::parse);
	}
};

template <typename T, typename ... TList>
struct DefaultParsersTuple<T, TList...> 
{
	using parsers_type = std::tuple<
		std::function<T(const std::string &)>,
		std::function<TList(const std::string &)>...>;

	static parsers_type get() 
	{
		return std::tuple_cat(
			std::forward_as_tuple(
				std::function<T(const std::string &)>(
					DefaultParser<T>::parse)),
			DefaultParsersTuple<TList...>::get());
	}
};

template <std::size_t N, typename ...Args>
struct TuplePrinter
{
	template<typename Ch, typename Tr>
	static void print(std::basic_ostream<Ch, Tr> &os, const std::tuple<Args...> &t)
	{
		TuplePrinter<N - 1, Args...>::print(os, t);
		return os << ", " << std::get<N - 1>(t);
	}
};

template <typename ...Args>
struct TuplePrinter<1, Args...>
{
	template<typename Ch, typename Tr>
	static void print(std::basic_ostream<Ch, Tr> &os, const std::tuple<Args...> &t)
	{
		return os << std::get<0>(t);
	}
};

template <typename Ch, typename Tr, typename ...Args>
std::basic_ostream<Ch, Tr> &operator <<(std::basic_ostream<Ch, Tr> &os, const std::tuple<Args...> &t)
{

	return TuplePrinter<sizeof...(Args), decltype(t)>::print(os << "(", t) << ")";
}

template <typename Row, typename Parsers, std::size_t N>
struct RowParser {
	static void parse(
		Row & row,
		const Parsers & parsers,
		const std::array<
		std::string,
		std::tuple_size<Parsers>::value
		> & parts) {

		std::get<N - 1>(row) = std::get<N - 1>(parsers)(parts[N - 1]);
		RowParser<Row, Parsers, N - 1>::parse(row, parsers, parts);
	}
};

template <typename Row, typename Parsers>
struct RowParser<Row, Parsers, 1> {
	static void parse(
		Row & row,
		const Parsers & parsers,
		const std::array<
		std::string,
		std::tuple_size<Parsers>::value
		> & parts) {

		std::get<0>(row) = std::get<0>(parsers)(parts[0]);
	}
};

template <typename Row, typename Parsers>
void parse_row(
	Row & row,
	const Parsers & parsers,
	const std::array<
	std::string,
	std::tuple_size<Parsers>::value
	> & parts) {

	RowParser<
		Row,
		Parsers,
		std::tuple_size<Parsers>::value
	>::parse(row, parsers, parts);
}

template <typename ... TList>
CSVReader<TList ...>::CSVReader() = default;

template <typename ... TList>
CSVReader<TList ...>::CSVReader(const char * filename)
	: stream_(filename) { }

template <typename ... TList>
CSVReader<TList ...>::CSVReader(const std::string & filename)
	: stream_(filename) { }

template <typename ... TList>
void CSVReader<TList ...>::open(const char * filename) {
	std::cout << "FF";
	stream_.open(filename);
	std::cout << "FF";
}

template <typename ... TList>
void CSVReader<TList ...>::open(const std::string & filename) {
	std::cout << "FF";
	stream_.open(filename);
	std::cout << "FF";
}

template <typename ... TList>
bool CSVReader<TList ...>::is_open() {
	return stream_.is_open();
}

template <typename ... TList>
bool CSVReader<TList ...>::is_open() const {
	return stream_.is_open();
}

template <typename ... TList>
char & CSVReader<TList ...>::separator() {
	return separator_;
}

template <typename ... TList>
const char & CSVReader<TList ...>::separator() const {
	return separator_;
}

template <typename ... TList>
char & CSVReader<TList ...>::quote() {
	return quote_;
}

template <typename ... TList>
const char & CSVReader<TList ...>::quote() const {
	return quote_;
}

template <typename ... TList>
bool & CSVReader<TList ...>::ignore_bad_row_length() {
	return ignore_bad_row_length_;
}

template <typename ... TList>
const bool & CSVReader<TList ...>::ignore_bad_row_length() const {
	return ignore_bad_row_length_;
}

template <typename ... TList>
bool & CSVReader<TList ...>::skip_empty_lines() {
	return skip_empty_lines_;
}

template <typename ... TList>
const bool & CSVReader<TList ...>::skip_empty_lines() const {
	return skip_empty_lines_;
}

template <typename ... TList>
std::size_t & CSVReader<TList ...>::skip_first_lines() {
	return skip_first_lines_;
}

template <typename ... TList>
const std::size_t & CSVReader<TList ...>::skip_first_lines() const {
	return skip_first_lines_;
}

template <typename ... TList>
typename CSVReader<TList...>::row_type CSVReader<TList ...>::operator[] (
	std::size_t i) {
	return rows_[i];
}

template <typename ... TList>
const typename CSVReader<TList...>::row_type CSVReader<TList ...>::operator[] (
	std::size_t i) const {
	return rows_[i];
}

template <typename ... TList>
std::size_t CSVReader<TList ...>::rows() const {
	return rows_.size();
}

template <typename ... TList>
std::size_t CSVReader<TList ...>::read_rows(std::size_t n) {
	std::size_t rows_readed = 0;
	std::string line;

	bool is_escaped;
	std::size_t k;
	std::size_t last_part;
	bool quoted;

	if (!first_lines_skipped_ && skip_first_lines_ > 0) {
		for (std::size_t i = 0; i < skip_first_lines_; ++i)
			std::getline(stream_, line);
		first_lines_skipped_ = true;
	}


	while (rows_readed < n) {
		if (!stream_)
			break;

		std::array<std::string, sizeof...(TList)> parts;

		std::getline(stream_, line);
		if (skip_empty_lines_ && line.empty())
			continue;

		row_type row;

		is_escaped = false;
		k = 0;
		last_part = 0;
		quoted = false;

		for (std::size_t i = 0; i < line.length(); ++i) {
			if (line[i] == escape_) {
				is_escaped = true;
				continue;
			}
			else is_escaped = false;

			if (!is_escaped && line[i] == quote_) {
				quoted = !quoted;
				continue;
			}

			if (!is_escaped && !quoted &&
				(line[i] == separator_ || i == line.length() - 1)) {

				if (k >= std::tuple_size<decltype(parsers_)>::value) {
					if (ignore_bad_row_length_)
						break;
					else
						throw std::runtime_error(
							"Bad row length. Expected " +
							std::to_string(sizeof...(TList)) +
							" got more.");
				}

				parts[k] = line.substr(	last_part, i - last_part + ((i == line.length() - 1) ? 1 : 0));
				last_part = i + 1;
				++k;
				continue;
			}
		}

		if (!ignore_bad_row_length_ &&
			k != std::tuple_size<decltype(parsers_)>::value)
			throw std::runtime_error(
				"Bad row length. Expected " +
				std::to_string(sizeof...(TList)) + " got " +
				std::to_string(k) + ".");

		for (std::size_t i = 0; i < parts.size(); ++i)
			std::cout << parts[i] << " ";
		std::cout << std::endl;

		parse_row(row, parsers_, parts);

		rows_.push_back(std::move(row));
		++rows_readed;
	}

	return rows_readed;
}

template <typename ... TList>
void CSVReader<TList ...>::close() {
	stream_.close();
}

int main() 
{
	std::ifstream file("test.csv");
	CSVReader<double, double, double> parser(file, 0);
	for (const std::tuple<double, double, double> &rs : parser)
	{
		std::cout << rs << std::endl;
	}
	system("pause");
}