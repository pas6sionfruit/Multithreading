#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>

std::vector<int> merge_sort(const std::vector<int>::iterator& begin,

	const std::vector<int>::iterator& end) {

	auto size_1 = end - begin, size_2 = size_1 / 2;
	auto temp_end = begin + size_2;

	if (size_1 < 2) {

		return std::vector<int>{*begin};
	}

	std::vector<int> result,
		part_1(merge_sort(begin, temp_end)),
		part_2(merge_sort(temp_end, end));

	result.reserve(part_1.size() + part_2.size());

	size_1 = part_1.size();
	size_2 = part_2.size();


	auto part_1_iterator = part_1.begin();
	auto part_2_iterator = part_2.begin();


	while (size_1 * size_2) {

		if (*part_1_iterator <= *part_2_iterator) {

			result.push_back(*part_1_iterator);
			++part_1_iterator;
			--size_1;
		}
		else {

			result.push_back(*part_2_iterator);
			++part_2_iterator;
			--size_2;
		}
	}

	if (size_1) {

		std::copy(part_1_iterator, part_1.end(), back_inserter(result));
	}

	else {

		std::copy(part_2_iterator, part_2.end(), back_inserter(result));
	}
	return result;
}


void merge_sort_paralel(const std::vector<int>::iterator& begin,

	const std::vector<int>::iterator& end,

	std::vector<int>& output) {

	if (begin == end) {

		return;
	}

	const size_t items_count = end - begin;

	constexpr size_t items_per_thread = 5;

	if (items_count <= items_per_thread) {

		output = merge_sort(begin, end);

		return;
	}

	std::vector<std::thread> threads;

	threads.reserve(items_count / items_per_thread);

	std::mutex mutex;

	for (size_t i = 0; i < items_count; i += items_per_thread) {

		size_t min = std::min(items_per_thread + i, items_count);

		threads.push_back(std::thread([begin = begin + i,

			end = begin + min, i, min, &mutex, &output]() {

			std::vector<int> temp(merge_sort(begin, end));

			std::lock_guard<std::mutex>lock_guard(mutex);


			for (size_t original_position = i, temp_position = 0;

				original_position < min; original_position++, temp_position++) {

				output[original_position] = temp[temp_position];
			}

		}));
	}

	auto merge = [&output](const std::vector<int>::iterator& begin,

		const std::vector<int>::iterator& end,

		const std::vector<int>::iterator& borderline) {

			for (auto i = begin; i != borderline;) {

				if (borderline != output.end() - 1 && borderline != end) {

					auto pos = borderline + 1;

					while (*(pos - 1) > *pos) {

						std::swap(*(pos - 1), *pos);

						if (pos < end - 1) ++pos;
					}
				}

				if (*i > *borderline) {

					std::swap(*i, *borderline);
				}

				else ++i;
			}
	};

	for (auto& i : threads) {

		i.join();
	}

	auto pairs = items_count / items_per_thread;

	for (size_t i = 0, j = 0; i < pairs; i++) {

		j += 5;

		merge(output.begin(),

			output.begin() + j, output.begin() + j - 5);
	}

	if (items_count % items_per_thread != 0) {

		merge(output.begin(), output.end(),

			output.end() - items_count % items_per_thread);

	}
}

bool check(std::vector<int>& v) {

	for (size_t i = 0; i < v.size() - 1; i++) {

		if (v[i] > v[i + 1]) {

			return false;
		}
	}
	return true;
}

int main() {

	std::vector<int>v, v2;

	for (int i = 0; i < 1000; i++) {

		size_t n = 1 + rand() % 1000;

		v.clear();

		std::cout << "element count: " << n << "\titeration number : ";

		while (n--) v.push_back((rand() % 1000) - 500);

		merge_sort_paralel(v.begin(), v.end(), v);

		if (check(v)) { //checking every vector

			std::cout << i << " \n";
		}
		else {

			std::cout << "ERROR! ";
			break;
		}
	}
}