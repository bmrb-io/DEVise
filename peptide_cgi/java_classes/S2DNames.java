// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Constant strings for names used in NMR-STAR files, etc.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1  2000/08/29 14:55:52  wenger
// Star2Devise can now extract relaxation parameters, H exchange rates, etc.
// from all appropriate NMR-STAR files, not just bmr4096.str (the sessions
// are not yet fully set up to handle this); fixed an error in the CSI
// calculations; improved test_summarize script.
//

// ========================================================================

public class S2DNames
{
    //
    // NMR-STAR tags and values.
    //
    public static final String ASSIGNED_CHEM_SHIFTS =
      "assigned_chemical_shifts";
    public static final String ATOM_NAME = "_Atom_name";
    public static final String ATOM_TYPE = "_Atom_type";
    public static final String CHEM_SHIFT_VALUE = "_Chem_shift_value";
    public static final String CONF_SUB_TOTAL =
      "_Conformer_submitted_total_number";
    public static final String COUPLING_CONSTANT_CODE =
      "_Coupling_constant_code";
    public static final String COUPLING_CONSTANTS = "coupling_constants";
    public static final String ENTRY_TITLE = "_Entry_title";
    public static final String H_EXCHANGE_PROT_FACT =
      "H_exchange_protection_factors";
    public static final String H_EXCHANGE_PROT_FACT_VALUE =
      "_H_exchange_protection_factor_value";
    public static final String H_EXCHANGE_RATE_VALUE =
      "_H_exchange_rate_value";
    public static final String H_EXCHANGE_RATE = "H_exchange_rate";
    public static final String H_EXCHANGE_RATES = "H_exchange_rates";
    public static final String HEME = "HEME";
    public static final String HETERONUCLEAR_NOE = "heteronuclear_NOE";
    public static final String MOL_POLYMER_CLASS = "_Mol_polymer_class";
    public static final String MOL_SYSTEM = "molecular_system";
    public static final String MOL_SYSTEM_NAME = "_Mol_system_name";
    public static final String MOL_SYS_COMP_NAME =
      "_Mol_system_component_name";
    public static final String RESIDUE_COUNT = "_Residue_count";
    public static final String RESIDUE_LABEL = "_Residue_label";
    public static final String RESIDUE_SEQ_CODE = "_Residue_seq_code";
    public static final String S2_PARAMS = "S2_parameters";
    public static final String SAVE_CONF_STAT =
      "save_conformer_statistical_characteristics";
    public static final String SAVE_DIST_CONSTRAINTS =
      "save_distance_constraints";
    public static final String SAVE_ENTRY_INFO = "save_entry_information";
    public static final String SAVE_GMG4 = "save_GMG4";
    public static final String SAVEFRAME_CATEGORY = "_Saveframe_category";
    public static final String SAVEFRAME_PREFIX = "_save";
    public static final String T1_RELAX = "T1_relaxation";
    public static final String T2_RELAX = "T2_relaxation";

    //
    // Atom names.
    //
    public static final String BACKBONE_ATOM_NAME = "CA";
    //TEMP -- add more here

    //
    // URL for getting NMR-STAR files from BMRB.
    //
    public static final String BMRB_STAR_URL =
      "http://www.bmrb.wisc.edu/data_library/files/";

    //
    // Data file suffixes.
    //
    public static final String CONSTRAINTS_DAT_SUFFIX = "i.dat";
    public static final String CSI_DAT_SUFFIX = "c.dat";
    public static final String DELTASHIFT_DAT_SUFFIX = "d.dat";
    public static final String PERCENT_ASSIGN_DAT_SUFFIX = "p.dat";
    public static final String T1_DAT_SUFFIX = "t1";
    public static final String T2_DAT_SUFFIX = "t2";

    //
    // Html file suffixes.
    //
    public static final String CHEM_SHIFT_HTML_SUFFIX = "c.html";
    public static final String COUPLING_HTML_SUFFIX = "g.html";
    public static final String DELTA_HTML_SUFFIX = "d.html";
    public static final String H_EXCH_HTML_SUFFIX = "h.html";
    public static final String ORDER_HTML_SUFFIX = "o.html";
    public static final String PCT_ASSIGNED_HTML_SUFFIX = "p.html";
    public static final String RELAX_HTML_SUFFIX = "r.html";
    public static final String SUMMARY_HTML_SUFFIX = "y.html";

    //
    // Helper files.
    //
    public static final String CHEMASSG_FILE = "assignments.txt";

}

// ========================================================================

