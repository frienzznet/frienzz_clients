<template>
	<v-container>
		<!-- Grid card layout für die gefundenen Einträge? Kuckst Du hier https://codepen.io/munieru_jp/pen/jpdJNV-->

		<v-toolbar>
			<v-btn icon @click="userpair_set_friend_properties(friend_info)" :disabled="!isFormValid">
				<v-icon>mdi-arrow-left-circle-outline</v-icon>
			</v-btn>
			<v-toolbar-title>{{ $t("main_title") }} &#x2022; {{ $t("edit_header") }} {{
				friend_info.original_friend_nickname
				}}</v-toolbar-title>

		</v-toolbar>
		<v-card>
			<v-form ref="form" v-model="isFormValid">
				<v-text-field v-model="friend_info.friend_nickname" :label="$t('edit_own_name')"
					:rules="[rules.required, rules.counter]"></v-text-field>
				<div v-if="!friend_info.public">
					<div>{{ $t("edit_group_help_text_head") }} {{ friend_info.original_friend_nickname }} {{
						$t("edit_group_help_text_tail") }} </div>
					<v-list>
						<v-list-item v-for="item in active_icons" :key="item.index">
							<v-list-item-title>
								<input type="checkbox" v-model="item.group_bit" v-bind:id="item.index"
									@change="update_friend_groups">
								<v-btn icon>
									<v-icon v-bind:color="item.color">{{ item.icon }}</v-icon>
								</v-btn>
							</v-list-item-title>
						</v-list-item>
					</v-list>
					<div style="color:red" v-if="friend_info.friend_groups == 0">{{ $t("edit_group_empty_warning_head")
						}}
						{{
							friend_info.original_friend_nickname }} {{ $t("edit_group_empty_warning_tail") }} </div>
				</div>
				<!--
				<v-btn text @click="userpair_set_friend_properties(friend_info)" :disabled="!isFormValid">
					{{ $t('edit_save') }}
				</v-btn>
				-->
				<v-divider></v-divider>
				<v-btn color=red @click="cardmenu_delete_dialog_show = friend_info.uuid">
					<v-icon>mdi-delete</v-icon>
					<div>{{ $t("cardmenu_delete_head") }} {{
						friend_info.original_friend_nickname }} {{ $t("cardmenu_delete_tail") }} </div>
				</v-btn>
			</v-form>
		</v-card>
		<v-dialog v-model="cardmenu_delete_dialog_show" scrollable max-width="300px">
			<v-card>
				<v-card-title>{{ $t("cardmenu_delete_dialog_header_head") + friend_info.friend_nickname +
					$t("cardmenu_delete_dialog_header_tail") }}</v-card-title>
				<v-divider></v-divider>
				<v-card-actions>
					<v-btn color="blue darken-1" text @click="cardmenu_delete_dialog_show = false">{{
						$t("cardmenu_delete_dialog_cancel") }}</v-btn>
					<v-btn color="red darken-1" text @click="userpair_unpair_friend(friend_info.uuid)">{{
						$t("cardmenu_delete_dialog_select") }}</v-btn>
				</v-card-actions>
			</v-card>
		</v-dialog>
	</v-container>
</template>

<script>
import router from "../router";
import messenger from "../messenger";

export default {
	name: "Edit",
	data() {
		return {
			isFormValid: false,
			available_icons: null,
			get_icon_for_available_type: null,
			friend_info: {
				friend_nickname: "Edit Abwesend",
				original_friend_nickname: "Edit Klickerklacker",
				info: "noch keine Info",
				available_time: 75 * 60,
				available_type: 1,
				description: "leere Beschreibung",
			},
			rules: {
				required: value => !!value || this.$t('required'),
				counter: value => !!value && value.length >= 5 || this.$t('edit_min_5_chars')
			},
			cardmenu_delete_dialog_show: false,
		}
	},
	computed: {
		active_icons: function () {
			var res = []
			this.available_icons.forEach(element => {
				if (element.index > 0) {
					res.push(element)
				}
			});
			return res
		},
	},
	created() {
		try {
			messenger.register("edit", this.messenger_onMessage, null, null);
			this.friend_info = this.$route.params.friend_info.friend_info
			this.available_icons = this.$route.params.friend_info.available_icons
			this.get_icon_for_available_type = this.$route.params.friend_info.get_icon_for_available_type
			this.overlay_friend_groups_bit()

			console.log("Edit loaded");
		} catch (error) {
			console.log("Edit exception", error);
			this.nav2Main();
		}
	},
	inject: ["available_icons", "get_icon_for_available_type", "get_icons_for_available_types"],
	methods: {
		nav2Main() {
			router.push({ name: "Home" }); // always goes 'back enough' to Main
		},
		messenger_onMessage(type, data) {
			console.log("incoming message to edit", type, data);
			/* 
			if (type == "just_a_dummy_for_later_extensions") {
	
			}
				*/
		},
		userpair_set_friend_properties(friend_info) {
			messenger.emit("userpair_set_friend_properties", {
				friend_info: friend_info,
			});
			this.nav2Main()
		},
		overlay_friend_groups_bit() {
			this.available_icons.forEach(icon => {
				if (icon.index > 0) {
					icon.group_bit = (this.friend_info.friend_groups & (2 ** icon.index)) != 0
				}
			});
		},
		update_friend_groups() {
			var res = 0
			this.available_icons.forEach(icon => {
				if (icon.index > 0 && icon.group_bit) {
					res += 2 ** icon.index
				}
			});
			this.friend_info.friend_groups = res
		},
		userpair_unpair_friend(uuid) {
			console.log("userpair_unpair_friend", uuid);
			this.cardmenu_delete_dialog_show = false
			messenger.emit("userpair_unpair_friend", {
				uuid: uuid
			});
		},
	}
}
</script>

<style scoped>
h1,
h2 {
	font-weight: normal;
}

ul {
	list-style-type: none;
	padding: 0;
}

li {
	display: inline-block;
	margin: 0 10px;
}

a {
	color: #42b983;
}
</style>
