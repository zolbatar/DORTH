struct Word
{
	bool (* compile)(std::list<Token>&, std::list<Token>::iterator&, CompilerLLVM&) = nullptr;
	void (* interpret)(CompilerLLVM& llvm) = nullptr;
};
