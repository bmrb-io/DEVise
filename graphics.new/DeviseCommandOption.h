#ifndef _DEVISECOMMAND_OPTION_H
#define _DEVISECOMMAND_OPTION_H
#include <string>
class DeviseCommandOption
{
	friend class CmdContainer;
	private:
		// only allow CmdContainer to modify its name
		void setCmdName(const string& name)
		{
			_cmdName = name;
		}
		void setCmdName(const char* name)
		{
			_cmdName = name;
		}
	public:
		DeviseCommandOption& operator =(DeviseCommandOption& obj)
		{
			_cmdName = obj._cmdName;
			return *this;
		}
		const string& getName() const
		{
			return (const string&)_cmdName;
		}
		void showName()
		{
			cout <<_cmdName;
		}
	private:
		string _cmdName;
};
#endif
