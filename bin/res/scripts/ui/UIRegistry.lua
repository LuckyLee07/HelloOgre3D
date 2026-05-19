local UIRegistry = {
	Act37TestView = {
		viewClass = "res.scripts.ui.views.Act37TestView",
		package = "act_37_test",
		component = "main_pifushoumai",
		x = 0,
		y = 0,
		layer = "Normal",
		cache = false,
	},

	Act37TestMvc = {
		package = "act_37_test",
		component = "main_pifushoumai",
		classlua = "Act37TestMvcAutoGen",
		x = 320,
		y = 0,
		layer = "Popup",
		cache = true,
		modal = false,
		requires = {
			"res.scripts.ui.views.Act37TestMvcCtrl",
			"res.scripts.ui.views.Act37TestMvcModel",
			"res.scripts.ui.views.Act37TestMvcView",
			"res.scripts.ui.views.Act37TestMvcAutoGen",
		},
	},
}

return UIRegistry
