#ifndef MMOPTIONMODEL_H_
#define MMOPTIONMODEL_H_

#include <wx/string.h>
#include <wx/arrstr.h>
#include <vector>

namespace SLGlobals {
const wxString nosection_name = _T("none");
const wxString nostyle_name = _T("none");
};
//! enum that lets us differentiate option types at runtime
/*! opt_undefined will be returned/set if the type could not be determined, others respectively */
enum OptionType {
	opt_undefined  = 0,
	opt_bool       = 1,
	opt_list       = 2,
	opt_float      = 3,
	opt_string     = 4,
	opt_int        = 5
};

//! used to hold an item in an option list
/*! An option list is made of a variable number of theses items.
 * Each item itself (should) contain a key, name and description.
 */
struct listItem
{
	listItem(wxString key_, wxString name_,wxString desc_);

		wxString key;
		wxString name;
		wxString desc;
};

//! Used in option list
typedef std::vector<listItem> ListItemVec;

//! The base class for all option types
/*! All members and functions are public (also in derived classes).
 * Therefore no sanity checks whatsoever are performed when changing a member.
 * Default constructors are mostly provided for compability with stl containers.
 */
struct mmOptionModel
{
    enum ControlType{
        ct_undefined,
        ct_someothers
    };


	//! sets members accordingly
	///* this ctor sets controltype enum according to string *///
	mmOptionModel(wxString name_, wxString key_, wxString description_, OptionType type_ = opt_undefined,
                wxString section_ = SLGlobals::nosection_name, wxString style_ = SLGlobals::nostyle_name);
    mmOptionModel(wxString name_, wxString key_, wxString description_, OptionType type_ = opt_undefined,
                wxString section_ = SLGlobals::nosection_name, ControlType style_ = ct_undefined);

	~mmOptionModel();
	//! all members are set to empty strings, type to opt_undefined
	mmOptionModel();

	wxString name, key, description,section;
	OptionType type;
	ControlType ct_type;
};

//! Holds a bool option
/*! difference from parent: members def and value are bool */
struct mmOptionBool : public mmOptionModel
{
	//! sets members accordingly
	mmOptionBool(wxString name_, wxString key_, wxString description_, bool def_,
                 wxString section_ = SLGlobals::nosection_name, wxString style_ = SLGlobals::nostyle_name);
	//! sets wxstring member to "" and bool members to false
	mmOptionBool();
	bool def;
	//! this will always represent the current value, also the only member that should change after creation
	bool value;
};

//! Holds a float option
struct mmOptionFloat : public mmOptionModel
{
	//! sets members accordingly
	mmOptionFloat(wxString name_, wxString key_, wxString description_, float def_, float stepping_, float min_, float max_,
                  wxString section_ = SLGlobals::nosection_name, wxString style_ = SLGlobals::nostyle_name);
	//! sets wxstring member to "" and float members to 0.0
	mmOptionFloat();

	float def;
	//! this will always represent the current value, also the only member that should change after creation
	float value;

	//! the increment with that value may change in min,max boundaries
	float stepping;
	float min, max;
};

//! Holds a string option
struct mmOptionString : public mmOptionModel
{
	//! sets members accordingly
	mmOptionString(wxString name_, wxString key_, wxString description_, wxString def_, unsigned int max_len_,
                   wxString section_ = SLGlobals::nosection_name, wxString style_ = SLGlobals::nostyle_name);
	//! sets wxstring member to "" and max_len to 0
	mmOptionString();

	//! should not exceed max length (not ensured)
	wxString def;
	//! this will always represent the current value,
	/*! the only member that should change after creation, before set check if new value exceeds max_len*/
	wxString value;
	//! the maximum lentgh the value string may have
	unsigned int max_len;
};

//! Holds a an option list (a vector of listItems)
/*! Most complex of option types. A convenience method for adding new Listitems is provided,
 * as well as a wxArrayString that contains the names of the added Listitems (useful for comboboxes)
 */
struct mmOptionList : public mmOptionModel
{
	//! sets members accordingly; listitems,cbx_choices remain empty
	mmOptionList(wxString name_, wxString key_, wxString description_, wxString def_,
                 wxString section_ = SLGlobals::nosection_name, wxString style_ = SLGlobals::nostyle_name);
	//! def, value are set to ""; listitems,cbx_choices remain empty
	mmOptionList();

	//! creates a new listitem from params, pushes it on the vector and adds name_ to cbx_choices
	void addItem(wxString key_, wxString name_, wxString desc_);

	wxString def;
	//! will always reflect the name of the currently "selected" listitem
	wxString value;
	//! index of current value in cbx_choices, so one can assign new combox value
	int cur_choice_index;
	//! holds a variable amount of ListItems
	ListItemVec listitems;
	//! for easy mapping to a combobox
	wxArrayString cbx_choices;

};

//! Holds a long integer option
struct mmOptionInt : public mmOptionModel
{
	//! sets members accordingly
	mmOptionInt(wxString name_, wxString key_, wxString description_, int def_, int stepping_, int min_, int max_,
                wxString section_ = SLGlobals::nosection_name, wxString style_ = SLGlobals::nostyle_name);
	//! sets wxstring member to "" and int members to 0
	mmOptionInt();

	long def;
	//! this will always represent the current value, also the only member that should change after creation
	long value;

	//! the increment with that value may change in min,max boundaries
	long stepping;
	long min, max;
};

#endif /*MMOPTIONMODEL_H_*/
