<!-- https://blog.kulturbanause.de/2013/12/css-grid-layout-module/ -->
<template>
	<transition>
		<div v-show="isVisible()" class="Frienzz-player">
			<div id="nickname">{{ user_info.nickname }}</div>
			<div id="info">{{ user_info.info }}</div>
			<!-- <div id="duration">{{ duration(user_info.available_time) }}</div> -->
			<div id="empty"></div>
			<div id="show">
				<v-btn icon @click="description_show = !description_show">
					<v-icon>{{
						description_show ? "mdi-chevron-up" : "mdi-chevron-down"
					}}</v-icon>
				</v-btn>
			</div>
			<div id="empty2"></div>
			<div id="slider">
				<v-slider v-model="user_info.available_time" max="28800" @click="user_set_available_time()">
				</v-slider>
			</div>
			<div id="time">&#x1F551;{{
				localDateTime(
					user_info.available_time + Math.floor((new Date()).getTime() / 1000),
					$t("locale_time_format")
				)
			}}</div>
			<div id="send">
				<v-btn icon v-if="needs_update()" @click="user_set_available_time()">
					<v-icon>mdi-send-outline</v-icon>
				</v-btn>
			</div>
			<div id="available_type">
				<avail-type-menu :item="user_info" />
			</div>

			<v-expand-transition>
				<div id="description" v-show="description_show">
					{{ user_info.description }}
				</div>
			</v-expand-transition>
		</div>
	</transition>
</template>
<script>
import messenger from "../messenger";
import dayjs from "dayjs";
import dayjsPluginUTC from "dayjs-plugin-utc";
import AvailTypeMenu from "./AvailTypeMenu.vue";

dayjs.extend(dayjsPluginUTC, { parseToLocal: true });

export default {
	name: "player",
	components: {
		AvailTypeMenu
	},
	data() {
		return {
			allow_to_show: true,
			available_time_old: 0,
			item: {},
			user_info: {
				nickname: "unbekannt",
				info: "noch keine Info",
				available_time: 75 * 60,
				available_type: 1,
				description: "leere Beschreibung",
			},
			description_show: false,
			available_type_dialog_show: false,
		};
	},
	created() {
		messenger.register("player_", this.messenger_onMessage, null, null);
	},
	methods: {
		isVisible() {
			return this.allow_to_show;
		},
		needs_update() {
			return this.available_time_old != this.user_info.available_time;
		},
		can_be_shown(show) {
			//get signaled from Home.vue if is ok to shown the player
			console.log("requested player show flag", show);
			this.allow_to_show = show;
		},
		messenger_onMessage(type, data) {
			console.log("incoming message to player", type, data);
			if (type == "player_user_info") {
				this.user_info = data;
			}

		},
		user_set_available_time() {
			if (this.available_time_old == this.user_info.available_time) {
				// for unknown reason this routine is triggered all the time- so we try to supress the effect here
				return;
			}
			console.log("Send user_set_available_time", this.user_info.available_time);
			messenger.emit("user_set_available_time", {
				available_time: this.user_info.available_time,
			});
			this.available_time_old = this.user_info.available_time;
		},
	},
	computed: {
	},
	inject: [
		'localDateTime',
		'duration',
	],
};
</script>

<style scoped>
.Frienzz-player {
	display: grid;
	max-width: 400px;
	/*
	grid-template-columns: 1fr 1fr 40px;
	*/
	grid-auto-columns: 40px 1fr 1fr 40px 40px;
	grid-template-rows: repeat(5, min-content);
	gap: 0px 0px;
	grid-template-areas:
		"nickname nickname nickname nickname nickname"
		"info info info info show"
		"time time duration empty empty"
		"empty2 slider slider send available_type"
		"description description description description description"
		"dialogs dialogs dialogs dialogs dialogs";
	background: rgb(189, 236, 178);
	color: black;
	border-radius: 10px;
	padding: 5px;
	margin-bottom: 10px;
	min-width: 98vw;
	max-width: 98vw;
}

#marker {
	background: rgb(255, 0, 0);
	grid-area: marker;
	border-radius: 5px;
}

#nickname {
	grid-area: nickname;
	font-family: "Atkinson-Hyperlegible", Helvetica, Arial;
	font-size: 200%;
	line-height: 100%;
	text-align: left;
	padding-left: 5px;
	font-weight: bold;
}

#info {
	grid-area: info;
	font-family: "Atkinson-Hyperlegible", Helvetica, Arial;
	font-size: 150%;
	text-align: left;
	padding-left: 5px;
}

#provider {
	color: black;
	font-weight: bold;
	grid-area: provider;
	text-align: left;
	padding-left: 5px;
}

#time {
	color: black;
	font-weight: bold;
	grid-area: time;
}

#duration {
	color: black;
	font-weight: bold;
	grid-area: duration;
}

#empty {
	grid-area: empty;
}

#empty2 {
	grid-area: empty2;
}


#available_type {
	grid-area: available_type;
}

#send {
	grid-area: send;
}

#show {
	grid-area: show;
}

#description {
	grid-area: description;
}

.v-icon {
	color: rgb(13, 65, 207);
}

.v-enter-active,
.v-leave-active {
	transition: opacity 0.5s ease;
	/*transition: max-height .5s;*/
}

.v-enter-from,
.v-leave-to {
	opacity: 0;
	/*max-height: 0;*/
}
</style>