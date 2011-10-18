/*
  I got this class from this URL: http://typethinker.blogspot.com/2010/05/timing-section-of-code-in-c.html

  It's public domain code. It works by you creating an instance of the
  object, with a string for what you're timing, and then when that
  object is destructed, it prints timing information.
 */

#include <iostream>
#include <ctime>

class TimedSection
{
	char const *d_name;
	timespec d_start;
	bool output_time;
	
public:
	
        TimedSection(char const *name, bool display_time) : d_name(name)
        {
		clock_gettime(CLOCK_REALTIME, &d_start);
		output_time = display_time;
        }
	
        ~TimedSection()
	{
		timespec end;
		clock_gettime(CLOCK_REALTIME, &end);
		double duration = 1e3 * (end.tv_sec - d_start.tv_sec) +
			1e-6 * (end.tv_nsec - d_start.tv_nsec);
		if (output_time)
		{
			std::cerr << d_name << '\t' << std::fixed << duration << " ms\n"; 	
		}
        }
};
