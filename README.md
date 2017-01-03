## Note
* No memory reclamation in code
* list1.hpp
	* one ad-hoc trick in program
	* when removing a node, next field of the node is changed to point at head node
	* this is needed because other transactions cannot detect the change of the field
* list2.hpp
	* next field of removed nodes is changed to nullptr
* implementation might be correct
	* not enough test, so possibly wrong
