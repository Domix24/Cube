#include <iostream>

#include "blocArray3d.h"
#include "blocinfo.h"
#include <iostream>
#include <string>
#include "define.h"



int main()
{
	//Test classe BlocInfo
	BlocInfo test(BlocType::BTYPE_GRASS, "Bloc de test");
	test.Afficher();

	//Test classe BlocArray3d
	BlocArray3d test2(200, 30, 40);
	test2.Reset(BlocType::BTYPE_GRASS);
	std::cout << test2.Get(2, 2, 2) << std::endl;


	
	system("pause");
	return 0;
}