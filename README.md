## Note
* one ad-hoc trick in program
	* when removing a node, next field of the node is changed to point at head node
	* this is needed because other transactions cannot detect the change of the field
* implementation might be correct
	* not enough test, so possibly wrong