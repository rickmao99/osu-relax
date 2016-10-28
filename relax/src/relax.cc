/*

	osu-relax - external 'relax-like' input simulator
	Read more at https://aixxe.net/2016/10/osu-game-hacking

	Copyright (C) 2016, aixxe. <me@aixxe.net>
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with osu-relax. If not, see <http://www.gnu.org/licenses/>.

*/

#include <thread>
#include <iostream>
#include "relax.h"

char left_click_key = 'z';
char right_click_key = 'x';

DWORD time_address = NULL;
DWORD game_process_id = NULL;
HANDLE game_process = NULL;

int wmain(int argc, wchar_t* argv[]) {
	// parse the beatmap we're going to play from argv.
	beatmap active_beatmap;
	
	if (argc < 2 || !active_beatmap.Parse(argv[1])) {
		std::cerr << "usage: " << argv[0] << " [beatmap]" << std::endl;
		return EXIT_FAILURE;
	}

	// find the process id of osu!
	game_process_id = get_process_id();

	if (!game_process_id) {
		std::cerr << "error: failed to find osu! process." << std::endl;
		return EXIT_FAILURE;
	}

	// open the process with memory reading access.
	game_process = OpenProcess(PROCESS_VM_READ, false, game_process_id);

	if (!game_process) {
		std::cerr << "error: failed to open handle to osu! process. (" << GetLastError() << ")" << std::endl;
		return false;
	}

	// find the actual time address.
	time_address = find_time_address();

	if (!time_address) {
		std::cerr << "error: failed to find pointer to time address." << std::endl;
		return EXIT_FAILURE;
	}

	// ready to go! wait for user input before starting.
	std::cout << "Ready to start! Press ENTER to continue." << std::endl;
	std::getchar();

	// define which object are we currently on.
	size_t current_object = 0;

	// iterate hitobjects to find out where we need to start from.
	int32_t time = get_elapsed_time();

	for (size_t i = 0; i < active_beatmap.hitobjects.size(); i++) {
		if (active_beatmap.hitobjects.at(i).start_time > time) {
			current_object = i;
			break;
		}
	}

	std::cout << "Starting from object #" << current_object << ".." << std::endl;

	// wait for the beatmap to start.
	while (current_object == 0 && get_elapsed_time() < active_beatmap.hitobjects.begin()->start_time) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	// get a reference to the current object
	hitobject& object = active_beatmap.hitobjects.at(current_object);
	
	// enter playback loop.
	while (current_object < active_beatmap.hitobjects.size()) {
		// define whether we've hit the current object.
		static bool key_down = false;

		// set the currently active key so we can alternate.
		static char active_key = left_click_key;

		// update the elapsed time.
		time = get_elapsed_time();

		if (time >= object.start_time - 5 && !key_down) {
			// hold key.
			set_key_pressed(active_key, true);
			key_down = true;

			continue;
		}
		
		if (time > object.end_time && key_down) {
			// release key.
			set_key_pressed(active_key, false);
			key_down = false;

			// advance to next object.
			current_object++;
			object = active_beatmap.hitobjects.at(current_object);
			
			// alternate keys. (completely aesthetic)
			active_key = (active_key == left_click_key) ? right_click_key : left_click_key;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	// clean up our handle and exit.
	CloseHandle(game_process);
	
	return EXIT_SUCCESS;
}