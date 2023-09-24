
# Loading additional proc with user specified bodies to compute parameter values.
source [file join [file dirname [file dirname [info script]]] gui/ms2xs_v2_0.gtcl]

# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  set C_DIN_AXIS_TDATA_WIDTH [ipgui::add_param $IPINST -name "C_DIN_AXIS_TDATA_WIDTH" -parent ${Page_0} -widget comboBox]
  set_property tooltip {AXI4Stream sink: Data Width} ${C_DIN_AXIS_TDATA_WIDTH}
  set C_DOUT_AXIS_TDATA_WIDTH [ipgui::add_param $IPINST -name "C_DOUT_AXIS_TDATA_WIDTH" -parent ${Page_0} -widget comboBox]
  set_property tooltip {Width of S_AXIS address bus. The slave accepts the read and write addresses of width C_M_AXIS_TDATA_WIDTH.} ${C_DOUT_AXIS_TDATA_WIDTH}
  set C_DOUT_AXIS_START_COUNT [ipgui::add_param $IPINST -name "C_DOUT_AXIS_START_COUNT" -parent ${Page_0}]
  set_property tooltip {Start count is the number of clock cycles the master will wait before initiating/issuing any transaction.} ${C_DOUT_AXIS_START_COUNT}
  #Adding Group
  set Arithmetic_Units [ipgui::add_group $IPINST -name "Arithmetic Units" -parent ${Page_0}]
  set_property tooltip {Number of Arithmetic Units} ${Arithmetic_Units}
  ipgui::add_param $IPINST -name "M" -parent ${Arithmetic_Units}

  #Adding Group
  set NTRU_Parameter_Set [ipgui::add_group $IPINST -name "NTRU Parameter Set" -parent ${Page_0} -layout horizontal]
  set_property tooltip {NTRU Parameter Set} ${NTRU_Parameter_Set}
  ipgui::add_param $IPINST -name "Std" -parent ${NTRU_Parameter_Set}
  set Std_PSet [ipgui::add_param $IPINST -name "Std_PSet" -parent ${NTRU_Parameter_Set} -widget comboBox]
  set_property tooltip {Standard Parameter Set} ${Std_PSet}

  #Adding Group
  set Parameters [ipgui::add_group $IPINST -name "Parameters" -parent ${Page_0}]
  set_property tooltip {NTRU Parameters} ${Parameters}
  set N [ipgui::add_param $IPINST -name "N" -parent ${Parameters}]
  set_property tooltip {NTRU N Parameter} ${N}
  set dr [ipgui::add_param $IPINST -name "dr" -parent ${Parameters}]
  set_property tooltip {NTRU dr Parameter} ${dr}
  set q [ipgui::add_param $IPINST -name "q" -parent ${Parameters}]
  set_property tooltip {NTRU q Parameter} ${q}
  set p [ipgui::add_param $IPINST -name "p" -parent ${Parameters}]
  set_property tooltip {NTRU p Parameter} ${p}



}

proc update_PARAM_VALUE.N { PARAM_VALUE.N PARAM_VALUE.Std PARAM_VALUE.Std_PSet } {
	# Procedure called to update N when any of the dependent parameters in the arguments change
	
	set N ${PARAM_VALUE.N}
	set Std ${PARAM_VALUE.Std}
	set Std_PSet ${PARAM_VALUE.Std_PSet}
	set values(Std) [get_property value $Std]
	set values(Std_PSet) [get_property value $Std_PSet]
	if { [gen_USERPARAMETER_N_ENABLEMENT $values(Std)] } {
		set_property enabled true $N
	} else {
		set_property enabled false $N
		set_property value [gen_USERPARAMETER_N_VALUE $values(Std_PSet)] $N
	}
}

proc validate_PARAM_VALUE.N { PARAM_VALUE.N } {
	# Procedure called to validate N
	return true
}

proc update_PARAM_VALUE.Std_PSet { PARAM_VALUE.Std_PSet PARAM_VALUE.Std } {
	# Procedure called to update Std_PSet when any of the dependent parameters in the arguments change
	
	set Std_PSet ${PARAM_VALUE.Std_PSet}
	set Std ${PARAM_VALUE.Std}
	set values(Std) [get_property value $Std]
	if { [gen_USERPARAMETER_Std_PSet_ENABLEMENT $values(Std)] } {
		set_property enabled true $Std_PSet
	} else {
		set_property enabled false $Std_PSet
	}
}

proc validate_PARAM_VALUE.Std_PSet { PARAM_VALUE.Std_PSet } {
	# Procedure called to validate Std_PSet
	return true
}

proc update_PARAM_VALUE.dr { PARAM_VALUE.dr PARAM_VALUE.Std PARAM_VALUE.Std_PSet } {
	# Procedure called to update dr when any of the dependent parameters in the arguments change
	
	set dr ${PARAM_VALUE.dr}
	set Std ${PARAM_VALUE.Std}
	set Std_PSet ${PARAM_VALUE.Std_PSet}
	set values(Std) [get_property value $Std]
	set values(Std_PSet) [get_property value $Std_PSet]
	if { [gen_USERPARAMETER_dr_ENABLEMENT $values(Std)] } {
		set_property enabled true $dr
	} else {
		set_property enabled false $dr
		set_property value [gen_USERPARAMETER_dr_VALUE $values(Std_PSet)] $dr
	}
}

proc validate_PARAM_VALUE.dr { PARAM_VALUE.dr } {
	# Procedure called to validate dr
	return true
}

proc update_PARAM_VALUE.p { PARAM_VALUE.p PARAM_VALUE.Std } {
	# Procedure called to update p when any of the dependent parameters in the arguments change
	
	set p ${PARAM_VALUE.p}
	set Std ${PARAM_VALUE.Std}
	set values(Std) [get_property value $Std]
	if { [gen_USERPARAMETER_p_ENABLEMENT $values(Std)] } {
		set_property enabled true $p
	} else {
		set_property enabled false $p
	}
}

proc validate_PARAM_VALUE.p { PARAM_VALUE.p } {
	# Procedure called to validate p
	return true
}

proc update_PARAM_VALUE.q { PARAM_VALUE.q PARAM_VALUE.Std } {
	# Procedure called to update q when any of the dependent parameters in the arguments change
	
	set q ${PARAM_VALUE.q}
	set Std ${PARAM_VALUE.Std}
	set values(Std) [get_property value $Std]
	if { [gen_USERPARAMETER_q_ENABLEMENT $values(Std)] } {
		set_property enabled true $q
	} else {
		set_property enabled false $q
	}
}

proc validate_PARAM_VALUE.q { PARAM_VALUE.q } {
	# Procedure called to validate q
	return true
}

proc update_PARAM_VALUE.M { PARAM_VALUE.M } {
	# Procedure called to update M when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.M { PARAM_VALUE.M } {
	# Procedure called to validate M
	return true
}

proc update_PARAM_VALUE.Std { PARAM_VALUE.Std } {
	# Procedure called to update Std when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.Std { PARAM_VALUE.Std } {
	# Procedure called to validate Std
	return true
}

proc update_PARAM_VALUE.C_DIN_AXIS_TDATA_WIDTH { PARAM_VALUE.C_DIN_AXIS_TDATA_WIDTH } {
	# Procedure called to update C_DIN_AXIS_TDATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_DIN_AXIS_TDATA_WIDTH { PARAM_VALUE.C_DIN_AXIS_TDATA_WIDTH } {
	# Procedure called to validate C_DIN_AXIS_TDATA_WIDTH
	return true
}

proc update_PARAM_VALUE.C_DOUT_AXIS_TDATA_WIDTH { PARAM_VALUE.C_DOUT_AXIS_TDATA_WIDTH } {
	# Procedure called to update C_DOUT_AXIS_TDATA_WIDTH when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_DOUT_AXIS_TDATA_WIDTH { PARAM_VALUE.C_DOUT_AXIS_TDATA_WIDTH } {
	# Procedure called to validate C_DOUT_AXIS_TDATA_WIDTH
	return true
}

proc update_PARAM_VALUE.C_DOUT_AXIS_START_COUNT { PARAM_VALUE.C_DOUT_AXIS_START_COUNT } {
	# Procedure called to update C_DOUT_AXIS_START_COUNT when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.C_DOUT_AXIS_START_COUNT { PARAM_VALUE.C_DOUT_AXIS_START_COUNT } {
	# Procedure called to validate C_DOUT_AXIS_START_COUNT
	return true
}


proc update_MODELPARAM_VALUE.C_DIN_AXIS_TDATA_WIDTH { MODELPARAM_VALUE.C_DIN_AXIS_TDATA_WIDTH PARAM_VALUE.C_DIN_AXIS_TDATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_DIN_AXIS_TDATA_WIDTH}] ${MODELPARAM_VALUE.C_DIN_AXIS_TDATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_DOUT_AXIS_TDATA_WIDTH { MODELPARAM_VALUE.C_DOUT_AXIS_TDATA_WIDTH PARAM_VALUE.C_DOUT_AXIS_TDATA_WIDTH } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_DOUT_AXIS_TDATA_WIDTH}] ${MODELPARAM_VALUE.C_DOUT_AXIS_TDATA_WIDTH}
}

proc update_MODELPARAM_VALUE.C_DOUT_AXIS_START_COUNT { MODELPARAM_VALUE.C_DOUT_AXIS_START_COUNT PARAM_VALUE.C_DOUT_AXIS_START_COUNT } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.C_DOUT_AXIS_START_COUNT}] ${MODELPARAM_VALUE.C_DOUT_AXIS_START_COUNT}
}

proc update_MODELPARAM_VALUE.N { MODELPARAM_VALUE.N PARAM_VALUE.N } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.N}] ${MODELPARAM_VALUE.N}
}

proc update_MODELPARAM_VALUE.q { MODELPARAM_VALUE.q PARAM_VALUE.q } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.q}] ${MODELPARAM_VALUE.q}
}

proc update_MODELPARAM_VALUE.p { MODELPARAM_VALUE.p PARAM_VALUE.p } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.p}] ${MODELPARAM_VALUE.p}
}

proc update_MODELPARAM_VALUE.dr { MODELPARAM_VALUE.dr PARAM_VALUE.dr } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.dr}] ${MODELPARAM_VALUE.dr}
}

proc update_MODELPARAM_VALUE.M { MODELPARAM_VALUE.M PARAM_VALUE.M } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.M}] ${MODELPARAM_VALUE.M}
}

proc update_MODELPARAM_VALUE.Std { MODELPARAM_VALUE.Std PARAM_VALUE.Std } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.Std}] ${MODELPARAM_VALUE.Std}
}

proc update_MODELPARAM_VALUE.Std_PSet { MODELPARAM_VALUE.Std_PSet PARAM_VALUE.Std_PSet } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.Std_PSet}] ${MODELPARAM_VALUE.Std_PSet}
}

