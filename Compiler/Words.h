struct Word
{
	bool (* compile)(std::list<Token>&, std::list<Token>::iterator&, CompilerLLVM&);
	void (* interpret)(CompilerLLVM& llvm);
};
