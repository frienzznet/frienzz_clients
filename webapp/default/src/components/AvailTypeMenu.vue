<template>
	<div class="Frienzz-availtypemenu">
		<v-menu bottom left rounded="pill" v-if="is_active()">
			<template v-slot:activator="{ on, attrs }">
				<v-btn dark icon v-bind="attrs" v-on="on">
					<v-icon
						size="400%" 
						:color="get_icon_for_available_type(item.available_type, true)">{{ get_icon_for_available_type(item.available_type, false) }}</v-icon>
				</v-btn>
			</template>

			<v-list color="grey darken+2">
				<v-list-item v-for="item in available_icons" :key="item.index">
					<v-list-item-title>
						<v-btn icon @click="user_set_available_type(item.index)">
							<v-icon v-bind:color="item.color">{{ item.icon }}</v-icon>
						</v-btn>
					</v-list-item-title>
				</v-list-item>
			</v-list>
		</v-menu>
	</div>
</template>
<script>

export default {
	name: "availtypemenu",

	props: {
		item: Object,
	},
	model: {
		prop: "list",
		event: "deleterequest",
	},
	data() {
		return {
			available_icons: null
		};
	},
	created() {
		this.available_icons = this.get_icons_for_available_types()
	},
	inject: ["user_set_available_type", "get_icons_for_available_types", "get_icon_for_available_type"],
	methods: {
		is_active() {
			return this.item.available_time > 0;
		},
	},
};
</script>

<style scoped></style>