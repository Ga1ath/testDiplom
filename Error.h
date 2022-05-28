#pragma once

class Error : public std::exception {
public:
	Error(const Coordinate& coord, const char* err) {
		msg = std::to_string(coord.line) + ":" + std::to_string(coord.pos) + ":" + err;
	}
	Error(const Coordinate& coord, const std::string& err) {
		msg = std::to_string(coord.line) + ":" + std::to_string(coord.pos) + ":" + err;
	}
	const char* what() {
		return msg.c_str();
	}
private:
	std::string msg;
};

