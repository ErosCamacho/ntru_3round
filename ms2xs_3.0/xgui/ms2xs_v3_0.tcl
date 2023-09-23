# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "C_DIN_AXIS_TDATA_WIDTH" -parent ${Page_0} -widget comboBox
  ipgui::add_param $IPINST -name "C_DOUT_AXIS_TDATA_WIDTH" -parent ${Page_0} -widget comboBox
  ipgui::add_param $IPINST -name "C_DOUT_AXIS_START_COUNT" -parent ${Page_0}
  #Adding Group
  set Arithmetic_Units [ipgui::add_group $IPINST -name "Arithmetic Units" -parent ${Page_0}]
  set_property tooltip {Number of Arithmetic Units} ${Arithmetic_Units}
  ipgui::add_param $IPINST -name "M" -parent ${Arithmetic_Units}

  #Adding Group
  set Parameters [ipgui::add_group $IPINST -name "Parameters" -parent ${Page_0}]
  set_property tooltip {NTRU Parameters} ${Parameters}
  set N [ipgui::add_param $IPINST -name "N" -parent ${Parameters}]
  set_property tooltip {NTRU N Parameter} ${N}
  set q [ipgui::add_param $IPINST -name "q" -parent ${Parameters}]
  set_property tooltip {NTRU q Parameter} ${q}
  set p [ipgui::add_param $IPINST -name "p" -parent ${Parameters}]
  set_property tooltip {NTRU p Parameter} ${p}
  ipgui::add_param $IPINST -name "nones" -parent ${Parameters}
  ipgui::add_param $IPINST -name "nminus" -parent ${Parameters}



}

proc update_PARAM_VALUE.M { PARAM_VALUE.M } {
	# Procedure called to update M when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.M { PARAM_VALUE.M } {
	# Procedure called to validate M
	return true
}

proc update_PARAM_VALUE.N { PARAM_VALUE.N } {
	# Procedure called to update N when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.N { PARAM_VALUE.N } {
	# Procedure called to validate N
	return true
}

proc update_PARAM_VALUE.nminus { PARAM_VALUE.nminus } {
	# Procedure called to update nminus when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.nminus { PARAM_VALUE.nminus } {
	# Procedure called to validate nminus
	return true
}

proc update_PARAM_VALUE.nones { PARAM_VALUE.nones } {
	# Procedure called to update nones when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.nones { PARAM_VALUE.nones } {
	# Procedure called to validate nones
	return true
}

proc update_PARAM_VALUE.p { PARAM_VALUE.p } {
	# Procedure called to update p when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.p { PARAM_VALUE.p } {
	# Procedure called to validate p
	return true
}

proc update_PARAM_VALUE.q { PARAM_VALUE.q } {
	# Procedure called to update q when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.q { PARAM_VALUE.q } {
	# Procedure called to validate q
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

proc update_MODELPARAM_VALUE.M { MODELPARAM_VALUE.M PARAM_VALUE.M } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.M}] ${MODELPARAM_VALUE.M}
}

proc update_MODELPARAM_VALUE.Std { MODELPARAM_VALUE.Std } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "Std". Setting updated value from the model parameter.
set_property value true ${MODELPARAM_VALUE.Std}
}

proc update_MODELPARAM_VALUE.Std_PSet { MODELPARAM_VALUE.Std_PSet } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	# WARNING: There is no corresponding user parameter named "Std_PSet". Setting updated value from the model parameter.
set_property value 2 ${MODELPARAM_VALUE.Std_PSet}
}

proc update_MODELPARAM_VALUE.nones { MODELPARAM_VALUE.nones PARAM_VALUE.nones } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.nones}] ${MODELPARAM_VALUE.nones}
}

proc update_MODELPARAM_VALUE.nminus { MODELPARAM_VALUE.nminus PARAM_VALUE.nminus } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.nminus}] ${MODELPARAM_VALUE.nminus}
}

