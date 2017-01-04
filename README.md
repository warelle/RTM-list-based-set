## Note
* No memory reclamation in code
* list1.hpp
	* next field of removed nodes is changed to nullptr
* list2.hpp
	* split each operation into segments
	* no adaptation or tuning, so expected too slow
* implementation might be correct
	* not enough test, so possibly wrong
