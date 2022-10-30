struct Word
{
	void (* compile)(std::list<Token>&, std::list<Token>::const_iterator, CompilerLLVM& llvm);
	void (* interpret)(CompilerLLVM& llvm);
};
