local GeneratedUIRegistry = {
-- <FairyGUIAutoGen:Act38Test>
	Act38Test = {
		package = "act_38_test",
		component = "QingLuanActMain",
		classlua = "Act38TestAutoGen",
		layer = "Popup",
		group = "Sample",
		scene = "Default",
		cache = false,
		fullScreen = true,
		requires = {
			"res.scripts.ui.views.Act38TestCtrl",
			"res.scripts.ui.views.Act38TestModel",
			"res.scripts.ui.views.Act38TestView",
			"res.scripts.ui.views.Act38TestAutoGen",
		},
	},
-- </FairyGUIAutoGen:Act38Test>
-- <FairyGUIAutoGen:Act37TestMvc>
	Act37TestMvc = {
		package = "act_37_test",
		component = "main_pifushoumai",
		classlua = "Act37TestMvcAutoGen",
		layer = "Popup",
		group = "Sample",
		scene = "Default",
		cache = true,
		requires = {
			"res.scripts.ui.views.Act37TestMvcCtrl",
			"res.scripts.ui.views.Act37TestMvcModel",
			"res.scripts.ui.views.Act37TestMvcView",
			"res.scripts.ui.views.Act37TestMvcAutoGen",
		},
	},
-- </FairyGUIAutoGen:Act37TestMvc>
-- <FairyGUIAutoGen:BusinessBenchmark>
	BusinessBenchmark = {
		package = "act_38_test",
		component = "QingLuanActMain",
		classlua = "BusinessBenchmarkAutoGen",
		layer = "Popup",
		group = "Benchmark",
		scene = "BusinessBenchmark",
		cache = false,
		fullScreen = true,
		closeOnEscape = true,
		requires = {
			"res.scripts.ui.views.BusinessBenchmarkCtrl",
			"res.scripts.ui.views.BusinessBenchmarkModel",
			"res.scripts.ui.views.BusinessBenchmarkView",
			"res.scripts.ui.views.BusinessBenchmarkAutoGen",
		},
	},
-- </FairyGUIAutoGen:BusinessBenchmark>
}

return GeneratedUIRegistry
