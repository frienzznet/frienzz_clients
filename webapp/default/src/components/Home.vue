// https://stackoverflow.com/questions/49501873/css-grid-items-based-on-minimum-width-and-percentage

<template>
	<v-container>
		<v-toolbar>
			<v-app-bar-nav-icon @click="nav2Set()">
				<v-icon>mdi-menu</v-icon>
			</v-app-bar-nav-icon>

			<v-toolbar-title>{{ $t("main_title") }}</v-toolbar-title>

			<v-spacer></v-spacer>

			<v-btn icon @click="nav2UserPair()">
				<v-icon>mdi-plus-circle</v-icon>
			</v-btn>
		</v-toolbar>

		<live-card v-for="friend_info in status.friends" :key="friend_info.uuid" :friend_info="friend_info"
			:max_seconds="status.max_seconds" />


		<v-footer app dark>
			<player ref="player" />
		</v-footer>
	</v-container>
</template>


<script>
import router from "../router";
import messenger from "../messenger";
import dayjs from "dayjs";
import dayjsPluginUTC from "dayjs-plugin-utc";
dayjs.extend(dayjsPluginUTC, { parseToLocal: true });
import LiveCard from "./LiveCard.vue";
import Player from "./Player.vue";

export default {
	name: "Frienzz",
	components: {
		LiveCard,
		Player,
	},
	title() {
		return `${this.name}`;
	},
	data() {
		return {
			last_window_scroll_pos: 0,
			lasttimer: Math.floor(new Date().getTime() / 1000), // stores time of last timer tick to proper calculate the time between
			last_refresh: 2, // stores time of last timer tick to proper calculate the time between. if this is < 0, then we trigger a refresh. Starts with 2, so that we have the first refresh already after 2 seconds and not after 5 minutes
			available_icons: [
				{
					color: "blue darken-1",
					icon: "mdi-radiobox-blank",
					index: 0
				},
				{
					color: "blue darken-1",
					icon: "mdi-phone-outline",
					index: 1
				},
				{
					color: "green darken-1",
					icon: "mdi-home-outline",
					index: 2
				},
				{
					color: "orange darken-1",
					icon: "mdi-coffee-outline",
					index: 3
				},
				{
					color: "yellow darken-1",
					icon: "mdi-glass-cocktail",
					index: 4
				},
				{
					color: "pink darken-1",
					icon: "mdi-food-turkey",
					index: 5
				},
				{
					color: "red darken-1",
					icon: "mdi-party-popper",
					index: 6
				}
			],
			status: {
				max_seconds: 8 * 3600,
				refresh_rate_secs: 5 * 60,
				refresh_rate_secs_active: 10,
				user_info: {
					username: "tim@example.com",
					nickname: "Nickname",
					info: "Info",
					available_time: 75 * 60,
					available_type: 1,
					description: "Beschreibung",
					public: false
				},
				friends: [
					{
						uuid: "1",
						friend_nickname: "Mustermann",
						original_friend_nickname: "Mustermann",
						public: true,
						info: "Ich bin ein Dummy",
						available_time: 8 * 0 * 60,
						available_type: 1,
						friend_groups: 7,
						description: "Etwas bla bla"
					},
					{
						uuid: "2",
						friend_nickname: "Klickerklacker",
						original_friend_nickname: "Klickerklacker",
						public: false,
						info: "Ich bin ein Dummiist",
						available_time: 90 * 60,
						available_type: 2,
						friend_groups: 5,
						description: "Lorem ipsum"
					},
					{
						uuid: "3",
						friend_nickname: "Abwesend",
						original_friend_nickname: "Klickerklacker",
						public: true,
						info: "Ich bin ein Dummiist",
						available_time: 0,
						available_type: 0,
						friend_groups: 5,
						description: "Lorem ipsum"
					},
				],
			}
		};
	},
	created() {
		this.tickTimer()
		messenger.register("home", this.messenger_onMessage, null, null);

		if (localStorage.userName && localStorage.token) {
			var token = localStorage.token;
			messenger.init(token);
			// send the token to the service worker, so that it can use it for the notifications
			const notification = {
				type: "token",
				token: token
			};
			// Get a reference to the service worker registration.
			navigator.serviceWorker.getRegistration().then(registration => {
				// Send the message to the service worker.
				// Check that the service worker registration exists.
				if (registration) {
					// Check that a service worker controller exists before
					// trying to access the postMessage method.
					if (navigator.serviceWorker.controller) {
						navigator.serviceWorker.controller.postMessage(notification);
					} else {
						console.log('No service worker controller found. Try a soft reload.');
					}
				}
			}).catch(error => {
				console.error('Error getting service worker registration:', error);
			});

		} else {
			this.nav2Login();
		}
		window.addEventListener("scroll", this.handleScroll);
	},
	mounted() {
		this.$refs.player.user_info = this.status.user_info
		this.$refs.player.available_time_old = this.status.user_info.available_time;
	},
	destroyed() {
		window.removeEventListener("scroll", this.handleScroll);
	},
	methods: {
		nav2Set() {
			router.push({ name: "Settings", params: { user_info: this.status.user_info } });
		},
		nav2Login() {
			router.push({ name: "Login" });
		},
		nav2UserPair() {
			router.push({ name: "UserPair" });
		},
		nav2Edit(friend_info) {
			console.log("click for edit", friend_info);
			router.push({ name: "Edit", params: { friend_info: friend_info } });
		},
		user_set_available_type(available_type) {
			this.status.user_info.available_type = available_type
			console.log("click for user_set_available_type", available_type);
			messenger.emit("user_set_available_type", { available_type: available_type });
		},
		messenger_onMessage(type, data) {
			console.log("incoming message to home", type, data);
			if (type == "home_status") {
				this.status = data.status;
				console.log("home this.status", this.status);
				// update player component
				messenger.handleMessage({ "type": "player_user_info", "config": data.status.user_info });
				// as this might be a reconnect, we switch the player component off, just in case that it is still open
				//this.$refs.player.can_be_shown(false)

			}
			if (type == "home_login") {
				this.nav2Login()
			}
		},
		localDateTime(timestamp, locale) {
			return dayjs.unix(timestamp).local().format(locale);
		},
		get_icons_for_available_types() {
			return this.available_icons
		},
		get_icon_for_available_type(state, color) {
			try {
				if (color) {
					return this.available_icons[state].color
				} else {
					return this.available_icons[state].icon
				}
			} catch (error) {
				if (color) {
					return "blue darken-1"
				} else {
					return "mdi-dots-vertical"
				}
			}
		},
		duration(secondsValue) {
			var seconds = parseInt(secondsValue, 10);
			if (!Number.isInteger(seconds) || seconds < 0) {
				return "";
			}
			if (seconds < 3600) {
				return dayjs.unix(seconds).utc().format("HH:mm");
				//return dayjs.unix(seconds).format("mm:ss");
			} else {
				return dayjs.unix(seconds).utc().format("HH:mm");
				//return dayjs.unix(seconds).format("HH:mm:ss");
			}
		},
		localMinutes(secondsValue) {
			var seconds = parseInt(secondsValue, 10);
			if (!Number.isInteger(seconds || seconds < 0)) {
				return "";
			}
			if (seconds < 3600) {
				return dayjs.unix(seconds).utc().format("mm [min]");
			} else {
				return dayjs.unix(seconds).utc().format("HH:mm");
			}
		},
		handleScroll() {
			this.$refs.player.can_be_shown(this.last_window_scroll_pos <= window.scrollY)
			this.last_window_scroll_pos = window.scrollY;
			//console.log(window.scrollY);
		},
		timerhook(delta_secs) { // triggered each timer tick
			let max_last_refresh = 0
			if (messenger.getState() == "active") {
				max_last_refresh = this.status.refresh_rate_secs_active
			} else {
				max_last_refresh = this.status.refresh_rate_secs
			}
			if (this.last_refresh > max_last_refresh) {
				this.last_refresh = max_last_refresh
			}
			for (const index in this.status.friends) {
				var friend = this.status.friends[index]
				if (friend.available_time > 0) {
					friend.available_time -= delta_secs // * 5 * 60
					if (friend.available_time < 0) {
						friend.available_time = 0
					}
				}
			}
			if (this.status.user_info.available_time > 0) {
				// TODO get the slider onclick() problem fixed on touch screens and remove the send button!
				var was_equal_before = false// a weird workaround to control the send button
				try {
					was_equal_before = this.$refs.player.available_time_old == this.status.user_info.available_time
				} catch (error) {
					console.log("catched error", error)
					was_equal_before = false
				}
				this.status.user_info.available_time -= delta_secs // * 5 * 60
				if (this.status.user_info.available_time < 0) {
					this.status.user_info.available_time = 0
				}
				if (was_equal_before) {
					try {
						this.$refs.player.available_time_old = this.status.user_info.available_time
					} catch (error) {
						console.log("catched error", error)
					}
				}
			}
			this.last_refresh -= delta_secs
			if (this.last_refresh < 0) {
				// it's time for a refresh
				if (messenger.getState() == "active") {
					this.last_refresh = this.status.refresh_rate_secs_active
				} else {
					this.last_refresh = this.status.refresh_rate_secs
				}
				console.log("automatic refresh with ", this.last_refresh, " secs on messenger state ", messenger.getState());
				messenger.emit("home_status", { host_status_type: this.$host_status_type_app });
			}
		},
		tickTimer() {
			var newtimer = Math.floor(new Date().getTime() / 1000)
			var delta_secs = newtimer - this.lasttimer
			this.timerhook(delta_secs)
			this.lasttimer = newtimer
			setTimeout(() => {
				this.tickTimer()
			}, 2000)
		}
	},
	watch: {
	},
	provide: function () {
		return {
			nav2Edit: this.nav2Edit,
			nav2Play: this.nav2Play,
			localDateTime: this.localDateTime,
			duration: this.duration,
			localMinutes: this.localMinutes,
			user_set_available_type: this.user_set_available_type,
			get_icon_for_available_type: this.get_icon_for_available_type,
			get_icons_for_available_types: this.get_icons_for_available_types
		};
	},
};
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
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
