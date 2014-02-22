/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#include "countrycodes.h"

#include <wx/intl.h>
#include <map>

static std::map<wxString,wxString> m_cc_table;

void InitTable()
{
	if (!m_cc_table.empty()) {
		return;
	}
	m_cc_table[_T("A1")] = _("Anonymous Proxy");
	m_cc_table[_T("A2")] = _("Satellite Provider");
	m_cc_table[_T("O1")] = _("Other Country");
	m_cc_table[_T("AD")] = _("Andorra");
	m_cc_table[_T("AE")] = _("United Arab Emirates");
	m_cc_table[_T("AF")] = _("Afghanistan");
	m_cc_table[_T("AG")] = _("Antigua and Barbuda");
	m_cc_table[_T("AI")] = _("Anguilla");
	m_cc_table[_T("AL")] = _("Albania");
	m_cc_table[_T("AM")] = _("Armenia");
	m_cc_table[_T("AN")] = _("Netherlands Antilles");
	m_cc_table[_T("AO")] = _("Angola");
	m_cc_table[_T("AQ")] = _("Antarctica");
	m_cc_table[_T("AR")] = _("Argentina");
	m_cc_table[_T("AS")] = _("American Samoa");
	m_cc_table[_T("AT")] = _("Austria");
	m_cc_table[_T("AU")] = _("Australia");
	m_cc_table[_T("AX")] = _("Åland Islands");
	m_cc_table[_T("AW")] = _("Aruba");
	m_cc_table[_T("AZ")] = _("Azerbaijan");
	m_cc_table[_T("BA")] = _("Bosnia and Herzegovina");
	m_cc_table[_T("BB")] = _("Barbados");
	m_cc_table[_T("BD")] = _("Bangladesh");
	m_cc_table[_T("BE")] = _("Belgium");
	m_cc_table[_T("BF")] = _("Burkina Faso");
	m_cc_table[_T("BG")] = _("Bulgaria");
	m_cc_table[_T("BH")] = _("Bahrain");
	m_cc_table[_T("BI")] = _("Burundi");
	m_cc_table[_T("BJ")] = _("Benin");
	m_cc_table[_T("BM")] = _("Bermuda");
	m_cc_table[_T("BN")] = _("Brunei Darussalam");
	m_cc_table[_T("BO")] = _("Bolivia");
	m_cc_table[_T("BR")] = _("Brazil");
	m_cc_table[_T("BS")] = _("Bahamas");
	m_cc_table[_T("BT")] = _("Bhutan");
	m_cc_table[_T("BV")] = _("Bouvet Island");
	m_cc_table[_T("BW")] = _("Botswana");
	m_cc_table[_T("BY")] = _("Belarus");
	m_cc_table[_T("BZ")] = _("Belize");
	m_cc_table[_T("CA")] = _("Canada");
	m_cc_table[_T("CC")] = _("Cocos (Keeling Islands)");
	m_cc_table[_T("CF")] = _("Central African Republic");
	m_cc_table[_T("CG")] = _("Congo");
	m_cc_table[_T("CH")] = _("Switzerland");
	m_cc_table[_T("CI")] = _("Cote D'Ivoire (Ivory Coast)");
	m_cc_table[_T("CK")] = _("Cook Islands");
	m_cc_table[_T("CL")] = _("Chile");
	m_cc_table[_T("CM")] = _("Cameroon");
	m_cc_table[_T("CN")] = _("China");
	m_cc_table[_T("CO")] = _("Colombia");
	m_cc_table[_T("CR")] = _("Costa Rica");
	m_cc_table[_T("CU")] = _("Cuba");
	m_cc_table[_T("CV")] = _("Cape Verde");
	m_cc_table[_T("CX")] = _("Christmas Island");
	m_cc_table[_T("CY")] = _("Cyprus");
	m_cc_table[_T("CZ")] = _("Czech Republic");
	m_cc_table[_T("DE")] = _("Germany");
	m_cc_table[_T("DJ")] = _("Djibouti");
	m_cc_table[_T("DK")] = _("Denmark");
	m_cc_table[_T("DM")] = _("Dominica");
	m_cc_table[_T("DO")] = _("Dominican Republic");
	m_cc_table[_T("DZ")] = _("Algeria");
	m_cc_table[_T("EC")] = _("Ecuador");
	m_cc_table[_T("EE")] = _("Estonia");
	m_cc_table[_T("EG")] = _("Egypt");
	m_cc_table[_T("EH")] = _("Western Sahara");
	m_cc_table[_T("ER")] = _("Eritrea");
	m_cc_table[_T("ES")] = _("Spain");
	m_cc_table[_T("ET")] = _("Ethiopia");
	m_cc_table[_T("FI")] = _("Finland");
	m_cc_table[_T("FJ")] = _("Fiji");
	m_cc_table[_T("FK")] = _("Falkland Islands (Malvinas)");
	m_cc_table[_T("FM")] = _("Micronesia");
	m_cc_table[_T("FO")] = _("Faroe Islands");
	m_cc_table[_T("FR")] = _("France");
	m_cc_table[_T("FX")] = _("France, Metropolitan");
	m_cc_table[_T("GA")] = _("Gabon");
	m_cc_table[_T("GD")] = _("Grenada");
	m_cc_table[_T("GE")] = _("Georgia");
	m_cc_table[_T("GF")] = _("French Guiana");
	m_cc_table[_T("GH")] = _("Ghana");
	m_cc_table[_T("GI")] = _("Gibraltar");
	m_cc_table[_T("GL")] = _("Greenland");
	m_cc_table[_T("GM")] = _("Gambia");
	m_cc_table[_T("GN")] = _("Guinea");
	m_cc_table[_T("GP")] = _("Guadeloupe");
	m_cc_table[_T("GQ")] = _("Equatorial Guinea");
	m_cc_table[_T("GR")] = _("Greece");
	m_cc_table[_T("GS")] = _("S. Georgia and S. Sandwich Isls.");
	m_cc_table[_T("GT")] = _("Guatemala");
	m_cc_table[_T("GU")] = _("Guam");
	m_cc_table[_T("GW")] = _("Guinea-Bissau");
	m_cc_table[_T("GY")] = _("Guyana");
	m_cc_table[_T("HK")] = _("Hong Kong");
	m_cc_table[_T("HM")] = _("Heard and McDonald Islands");
	m_cc_table[_T("HN")] = _("Honduras");
	m_cc_table[_T("HR")] = _("Croatia (Hrvatska)");
	m_cc_table[_T("HT")] = _("Haiti");
	m_cc_table[_T("HU")] = _("Hungary");
	m_cc_table[_T("ID")] = _("Indonesia");
	m_cc_table[_T("IE")] = _("Ireland");
	m_cc_table[_T("IL")] = _("Israel");
	m_cc_table[_T("IN")] = _("India");
	m_cc_table[_T("IO")] = _("British Indian Ocean Territory");
	m_cc_table[_T("IQ")] = _("Iraq");
	m_cc_table[_T("IR")] = _("Iran");
	m_cc_table[_T("IS")] = _("Iceland");
	m_cc_table[_T("IT")] = _("Italy");
	m_cc_table[_T("JM")] = _("Jamaica");
	m_cc_table[_T("JO")] = _("Jordan");
	m_cc_table[_T("JP")] = _("Japan");
	m_cc_table[_T("KE")] = _("Kenya");
	m_cc_table[_T("KG")] = _("Kyrgyzstan (Kyrgyz Republic)");
	m_cc_table[_T("KH")] = _("Cambodia");
	m_cc_table[_T("KI")] = _("Kiribati");
	m_cc_table[_T("KM")] = _("Comoros");
	m_cc_table[_T("KN")] = _("Saint Kitts and Nevis");
	m_cc_table[_T("KP")] = _("Korea (North) (People's Republic)");
	m_cc_table[_T("KR")] = _("Korea (South) (Republic)");
	m_cc_table[_T("KW")] = _("Kuwait");
	m_cc_table[_T("KY")] = _("Cayman Islands");
	m_cc_table[_T("KZ")] = _("Kazakhstan");
	m_cc_table[_T("LA")] = _("Laos");
	m_cc_table[_T("LB")] = _("Lebanon");
	m_cc_table[_T("LC")] = _("Saint Lucia");
	m_cc_table[_T("LI")] = _("Liechtenstein");
	m_cc_table[_T("LK")] = _("Sri Lanka");
	m_cc_table[_T("LR")] = _("Liberia");
	m_cc_table[_T("LS")] = _("Lesotho");
	m_cc_table[_T("LT")] = _("Lithuania");
	m_cc_table[_T("LU")] = _("Luxembourg");
	m_cc_table[_T("LV")] = _("Latvia");
	m_cc_table[_T("LY")] = _("Libya");
	m_cc_table[_T("MA")] = _("Morocco");
	m_cc_table[_T("MC")] = _("Monaco");
	m_cc_table[_T("MD")] = _("Moldova");
	m_cc_table[_T("ME")] = _("Montenegro");
	m_cc_table[_T("MG")] = _("Madagascar");
	m_cc_table[_T("MH")] = _("Marshall Islands");
	m_cc_table[_T("MK")] = _("Macedonia");
	m_cc_table[_T("ML")] = _("Mali");
	m_cc_table[_T("MM")] = _("Myanmar");
	m_cc_table[_T("MN")] = _("Mongolia");
	m_cc_table[_T("MO")] = _("Macau");
	m_cc_table[_T("MP")] = _("Northern Mariana Islands");
	m_cc_table[_T("MQ")] = _("Martinique");
	m_cc_table[_T("MR")] = _("Mauritania");
	m_cc_table[_T("MS")] = _("Montserrat");
	m_cc_table[_T("MT")] = _("Malta");
	m_cc_table[_T("MU")] = _("Mauritius");
	m_cc_table[_T("MV")] = _("Maldives");
	m_cc_table[_T("MW")] = _("Malawi");
	m_cc_table[_T("MX")] = _("Mexico");
	m_cc_table[_T("MY")] = _("Malaysia");
	m_cc_table[_T("MZ")] = _("Mozambique");
	m_cc_table[_T("NA")] = _("Namibia");
	m_cc_table[_T("NC")] = _("New Caledonia");
	m_cc_table[_T("NE")] = _("Niger");
	m_cc_table[_T("NF")] = _("Norfolk Island");
	m_cc_table[_T("NG")] = _("Nigeria");
	m_cc_table[_T("NI")] = _("Nicaragua");
	m_cc_table[_T("NL")] = _("Netherlands");
	m_cc_table[_T("NO")] = _("Norway");
	m_cc_table[_T("NP")] = _("Nepal");
	m_cc_table[_T("NR")] = _("Nauru");
	m_cc_table[_T("NT")] = _("Neutral Zone (Saudia Arabia/Iraq)");
	m_cc_table[_T("NU")] = _("Niue");
	m_cc_table[_T("NZ")] = _("New Zealand");
	m_cc_table[_T("OM")] = _("Oman");
	m_cc_table[_T("PA")] = _("Panama");
	m_cc_table[_T("PE")] = _("Peru");
	m_cc_table[_T("PF")] = _("French Polynesia");
	m_cc_table[_T("PG")] = _("Papua New Guinea");
	m_cc_table[_T("PH")] = _("Philippines");
	m_cc_table[_T("PK")] = _("Pakistan");
	m_cc_table[_T("PL")] = _("Poland");
	m_cc_table[_T("PM")] = _("St. Pierre and Miquelon");
	m_cc_table[_T("PN")] = _("Pitcairn");
	m_cc_table[_T("PR")] = _("Puerto Rico");
	m_cc_table[_T("PT")] = _("Portugal");
	m_cc_table[_T("PW")] = _("Palau");
	m_cc_table[_T("PY")] = _("Paraguay");
	m_cc_table[_T("QA")] = _("Qatar");
	m_cc_table[_T("RE")] = _("Reunion");
	m_cc_table[_T("RO")] = _("Romania");
	m_cc_table[_T("RS")] = _("Serbia");
	m_cc_table[_T("RU")] = _("Russian Federation");
	m_cc_table[_T("RW")] = _("Rwanda");
	m_cc_table[_T("SA")] = _("Saudi Arabia");
	m_cc_table[_T("SB")] = _("Solomon Islands");
	m_cc_table[_T("SC")] = _("Seychelles");
	m_cc_table[_T("SD")] = _("Sudan");
	m_cc_table[_T("SE")] = _("Sweden");
	m_cc_table[_T("SG")] = _("Singapore");
	m_cc_table[_T("SH")] = _("St. Helena");
	m_cc_table[_T("SI")] = _("Slovenia");
	m_cc_table[_T("SJ")] = _("Svalbard and Jan Mayen Islands");
	m_cc_table[_T("SK")] = _("Slovakia (Slovak Republic)");
	m_cc_table[_T("SL")] = _("Sierra Leone");
	m_cc_table[_T("SM")] = _("San Marino");
	m_cc_table[_T("SN")] = _("Senegal");
	m_cc_table[_T("SO")] = _("Somalia");
	m_cc_table[_T("SR")] = _("Suriname");
	m_cc_table[_T("ST")] = _("Sao Tome and Principe");
	m_cc_table[_T("SU")] = _("Soviet Union (former)");
	m_cc_table[_T("SV")] = _("El Salvador");
	m_cc_table[_T("SY")] = _("Syria");
	m_cc_table[_T("SZ")] = _("Swaziland");
	m_cc_table[_T("TC")] = _("Turks and Caicos Islands");
	m_cc_table[_T("TD")] = _("Chad");
	m_cc_table[_T("TF")] = _("French Southern Territories");
	m_cc_table[_T("TG")] = _("Togo");
	m_cc_table[_T("TH")] = _("Thailand");
	m_cc_table[_T("TJ")] = _("Tajikistan");
	m_cc_table[_T("TK")] = _("Tokelau");
	m_cc_table[_T("TM")] = _("Turkmenistan");
	m_cc_table[_T("TN")] = _("Tunisia");
	m_cc_table[_T("TO")] = _("Tonga");
	m_cc_table[_T("TP")] = _("East Timor");
	m_cc_table[_T("TR")] = _("Turkey");
	m_cc_table[_T("TT")] = _("Trinidad and Tobago");
	m_cc_table[_T("TV")] = _("Tuvalu");
	m_cc_table[_T("TW")] = _("Taiwan");
	m_cc_table[_T("TZ")] = _("Tanzania");
	m_cc_table[_T("UA")] = _("Ukraine");
	m_cc_table[_T("UG")] = _("Uganda");
	m_cc_table[_T("GB")] = _("United Kingdom");
	m_cc_table[_T("UM")] = _("US Minor Outlying Islands");
	m_cc_table[_T("US")] = _("United States");
	m_cc_table[_T("UY")] = _("Uruguay");
	m_cc_table[_T("UZ")] = _("Uzbekistan");
	m_cc_table[_T("VA")] = _("Vatican City State (Holy See)");
	m_cc_table[_T("VC")] = _("Saint Vincent and The Grenadines");
	m_cc_table[_T("VE")] = _("Venezuela");
	m_cc_table[_T("VG")] = _("Virgin Islands (British)");
	m_cc_table[_T("VI")] = _("Virgin Islands (US)");
	m_cc_table[_T("VN")] = _("Viet Nam");
	m_cc_table[_T("VU")] = _("Vanuatu");
	m_cc_table[_T("WF")] = _("Wallis and Futuna Islands");
	m_cc_table[_T("WS")] = _("Samoa");
	m_cc_table[_T("YE")] = _("Yemen");
	m_cc_table[_T("YT")] = _("Mayotte");
	m_cc_table[_T("YU")] = _("Yugoslavia");
	m_cc_table[_T("ZA")] = _("South Africa");
	m_cc_table[_T("ZM")] = _("Zambia");
	m_cc_table[_T("ZR")] = _("Zaire");
	m_cc_table[_T("ZW")] = _("Zimbabwe");
}

wxString GetFlagNameFromCountryCode( const wxString& cc )
{
	InitTable();
	wxString ret = m_cc_table[ cc ];
	if ( ret.IsEmpty() ) {
		return cc + _T(" ") + _("(Full country name not found)");
	}
	return ret;
}
