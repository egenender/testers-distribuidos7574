#ifndef CONFIG_H
#define	CONFIG_H

#include <map>
#include <string>

class Config {
	public:
		/* Crate configuration object, reading from `filename` using `maxLineLength`
		 * maxLineLength is optional and defaults to 256 */
		Config(std::string filename, unsigned maxLineLength = 256);
		/* Read the string `key` from the configuration, if absent return `defaultvalue`*/
		std::string getString(std::string key, std::string defaultValue);
		/* Read the integer `key` from the configuration, if absent return `defaultvalue`*/
		int getInt(std::string key, int defaultValue);
	private:
		std::map<std::string, std::string> properties;
		std::string trim(std::string s);
};

#endif	/* CONFIG_H */

