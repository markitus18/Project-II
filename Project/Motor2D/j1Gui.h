#pragma region Commands
	struct C_UIDebug : public Command
	{
		C_UIDebug() : Command("gui_debug", "Enable / Disable GUI debug", 1, NULL, "GUI"){}
		void function(const C_DynArray<C_String>* arg);
	};
	C_UIDebug c_UIDebug;
#pragma endregion