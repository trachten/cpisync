## <CPISync/Data/>
* **DataObject.h**
    * Data objects are wrappers for a generic set element to be synchronized
    * `DataObject(data)`
        * Create a data object by passing in a variety of different data types (ZZ,string,multiset,etc.)
        * See [DataObject.h](../../include/CPISync/Data/DataObject.h) for a complete list of constructors
    * `DataObject.to_ZZ()`
        * Return the data contained in your DataObject as a ZZ type
    * `DataObject.to_string()`
        * Return the data contained in your DataObject as a string
    * `DataObject.to_char_array()`
        * Return A char array version of the contents of this data object. The string could have null bytes and non-printable characters
    * `DataObject.to_Set()`
        * Deserialize and return a multiset
    * `DataObject.print()`
        * Returns a base 64 encoded string of the internal contents of the DataObject
    * `DataObject.setTimeStamp(clock_t time)`
        * Set the time of creation for this DatObject. This is also done in the constructor
    * `DataObject.getTimeStamp()`
        * Returns the time of creation or the setTimeStamp of the DataObject
    *   `<< opperator`
        * Sends the `to_string()` representation of the contents of the DataObject to the ostream
    * `== opperator`
        * compares the to_ZZ representations of two data objects

* **DataPriorityObject.h**
    * TODO: FILL THIS SECTION
