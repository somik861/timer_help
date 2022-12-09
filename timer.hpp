#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <string_view>

class _TimerRaii {
  public:
	_TimerRaii(const std::string& id);
	~_TimerRaii();

	_TimerRaii(const _TimerRaii&) = delete;
	_TimerRaii& operator=(const _TimerRaii&) = delete;

	_TimerRaii(_TimerRaii&&) = delete;
	_TimerRaii& operator=(_TimerRaii&&) = delete;

  private:
	std::string _id;
};

class _Timer {
  public:
	void set_output(const std::filesystem::path& p) { _output = p; }

	void start(const std::string& id) {
		auto& val = _starts[id];
		val = now();
	}

	_TimerRaii start_raii(const std::string& id) { return _TimerRaii(id); }

	void end(const std::string& id) {
		auto val = now();

		auto& start = _starts.at(id);
		_summary[id] += val - start;
		++_counts[id];
	}

	void flush() const {
		if (_output.empty())
			flush_to(std::cout);
		else
			flush_to(_output);
	}
	void flush_to(const std::filesystem::path& p) const {
		std::ofstream f(p);
		flush_to(f);
	}
	void flush_to(std::ostream& p) const {
		p << "Time consumned by sections:\n";
		for (const auto& [id, time] : _summary)
			p << id << ":  " << time.count() / 1'000'000'000.0l << "s"
			  << "     (" << time.count() / _counts.at(id) / 1'000'000.0l
			  << "ms per cycle)"
			  << "\n";
	}

	void clear() {
		_summary.clear();
		_starts.clear();
	}

	~_Timer() {
		if (!_summary.empty())
			flush();
	}

  private:
	std::chrono::time_point<std::chrono::steady_clock> now() const {
		return std::chrono::steady_clock::now();
	}

	std::filesystem::path _output;
	std::map<std::string, std::chrono::duration<long int, std::nano>> _summary;
	std::map<std::string, std::chrono::time_point<std::chrono::steady_clock>>
	    _starts;

	std::map<std::string, std::size_t> _counts;
};

inline _Timer Timer;

inline _TimerRaii::_TimerRaii(const std::string& id) : _id(id) {
	Timer.start(id);
}

inline _TimerRaii::~_TimerRaii() { Timer.end(_id); }
