#ifndef _DEVISEKEYBINDING_H_
#define _DEVISEKEYBINDING_H_

class DeviseKeyBinding
{
  public:

    DeviseKeyBinding();

    ~DeviseKeyBinding() {}

    // returns previous action
    int Bind(int key, int action);
    int Unbind(int key);

  protected:

    enum { MAX_BINDINGS = 103; }

    struct Binding {
	int key;
	int action;
    };

    static Binding _default_binding[];

    MapIntToInt _key_binding;

  private:

    // N/A
    DeviseKeyBinding(const DeviseKeyBinding& other);
    DeviseKeyBinding& operator=(const DeviseKeyBinding& other);
};




#endif // _DEVISEKEYBINDING_H_
