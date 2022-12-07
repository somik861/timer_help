#include "timer.hpp"

int main() {
	Timer.start("Huuu");

    {
        Timer.start_raii("RaiiHUUU");
    }

	Timer.end("Huuu");
}
