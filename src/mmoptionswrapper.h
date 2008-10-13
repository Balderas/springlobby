#ifndef MMOPTIONSORAPPER_H_
#define MMOPTIONSORAPPER_H_

#include "iunitsync.h"

#include <vector>
#include <utility>
#include <wx/string.h>


typedef std::pair < wxString,wxString> wxStringPair;
typedef std::pair < wxString, wxStringPair> wxStringTriple;
typedef std::vector<wxStringPair> wxStringPairVec;
typedef std::vector<wxStringTriple> wxStringTripleVec;
typedef std::map<wxString,wxString> wxStringMap;

struct GameOptions;

//! enum to differentiate option category easily at runtime
enum GameOption{
  PrivateOptions  = 3,
  EngineOption = 2,
	MapOption    = 1,
	ModOption    = 0,
	LastOption = 4
};// should reflect: optionCategoriesCount

class mmOptionsWrapper
{
public:
	//does nothing
	mmOptionsWrapper();
	virtual ~mmOptionsWrapper();
	//! just calls loadOptions(MapOption,mapname)
	bool loadMapOptions(wxString mapname);
	//! obsolete
	bool reloadMapOptions(wxString mapname);
	//! load corresponding options through unitsync calls
	/*!
	 * the containers for corresponding flag are recreated and then gets the number of options from unitsync
	 * and adds them one by one  to the appropiate container
	 * \param flag decides which type of option to load
	 * \return true if load successful, false otherwise
	 */
	bool loadOptions(GameOption flag, wxString name = _T(""));
	//! checks if given key can be found in specified container
	/*!
	 * \param key the key that should be checked for existance in containers
	 * \param flag which GameOption conatiner should be searched
	 * \param showError if true displays a messagebox if duplicate key is found
	 * \param optType will contain the corresponding OptionType if key is found, opt_undefined otherwise
	 * \return true if key is found, false otherwise
	 */
	bool keyExists(wxString key,GameOption flag,bool showError, OptionType& optType) const ;
	//! given a vector of key/value pairs sets the appropiate options to new values
	/*!	Every new value is tested for meeting boundary conditions, type, etc.
	 * If test fails error is logged and false is returned.
	 * \param values the wxStringPairVec containing key / new value pairs
	 * \param flag which OptionType is to be processed
	 * \return false if ANY error occured, true otherwise
	 */
	bool setOptions(wxStringPairVec* values,GameOption flag);
	//! get all options of one GameOption
	/*! the output has the following format: < wxString , Pair < wxString , wxString > >
	 * meaning < key , < name , value > >
	 * \param triples this will contain the options after the function
	 * \param flag which OptionType is to be processed
	 */
	wxStringTripleVec getOptions( GameOption flag ) const ;
	//! similar to getOptions, instead of vector a map is used and the name is not stored
	std::map<wxString,wxString> getOptionsMap(GameOption) const ;
	//! recreates ALL containers
	void unLoadOptions();
	//! recreates the containers of corresponding flag
	void unLoadOptions(GameOption flag);

	//! returns value of specified key
	/*! searches all containers for key
	 * \return value of key if key found, "" otherwise
	 */
	wxString getSingleValue(wxString key) const ;
	//! returns value of specified key
	/*! searches containers of type flag for key
	 * \return value of key if key found, "" otherwise
	 */

	wxString getSingleValue(wxString key, GameOption flag) const;

	//! sets a single option in specified container
	/*! \return true if success, false otherwise */
	bool setSingleOption(wxString key, wxString value, GameOption modmapFlag);
	//! same as before, but tries all containers
	bool setSingleOption(wxString key, wxString value);

	//! returns the option type of specified key (all containers are tried)
	OptionType GetSingleOptionType (wxString key) const ;

	//!returns the cbx_choice associated w current listoption
	wxString GetNameListOptValue(wxString key, GameOption flag) const;

	//! returns the listitem key associated with listitem name
	wxString GetNameListOptItemKey(wxString optkey, wxString itemname, GameOption flag) const ;

//private:
	const static int optionCategoriesCount = 4;
	GameOptions opts[optionCategoriesCount];
protected:
	//! used for code clarity in setOptions()
	bool setSingleOptionTypeSwitch(wxString key, wxString value, GameOption modmapFlag, OptionType optType);

};

#endif /*MMOPTIONSORAPPER_H_*/
