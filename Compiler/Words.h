struct Word
{
	void (* compile)(std::list<Token>&, std::list<Token>::const_iterator);
	void (* interpret)(void);
};
