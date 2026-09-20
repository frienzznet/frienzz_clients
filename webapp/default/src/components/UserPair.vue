<template>
	<!-- Grid card layout für die gefundenen Einträge? Kuckst Du hier https://codepen.io/munieru_jp/pen/jpdJNV-->

	<v-container>
		<v-toolbar>
			<v-app-bar-nav-icon @click="nav2Main()">
				<v-icon>mdi-arrow-left-circle-outline</v-icon>
			</v-app-bar-nav-icon>
			<v-toolbar-title>{{ $t("main_title") }} &#x2022; {{ $t("userpair_select_header") }}</v-toolbar-title>
		</v-toolbar>
		<v-card class="mx-auto">
			<v-form ref="userpair_select">
				<v-row>
					<v-col>
						{{ this.invitation }}
					</v-col>
				</v-row>
				<v-row>
					<v-col>
						<v-btn @click="userpair_request_invitation()">
							{{ $t('userpair_generate_friend_number') }}
						</v-btn>
					</v-col>
				</v-row>
				<v-row>
					<v-col>
						{{ $t(this.instruction) }}
					</v-col>
				</v-row>
				<v-row>
					<v-col>
						<v-card class="text-center d-flex flex-column align-center justify-center">

							<div id="qrcode"></div>

						</v-card>
					</v-col>
				</v-row>
				<v-row>
					<v-col>
						<v-btn v-if="this.able_to_share" @click="share_invitation()">
							<v-icon left> mdi-share-variant </v-icon>
							{{ $t('userpair_share_invitation') }}
						</v-btn>
						<v-btn v-if="!this.able_to_share" @click="copy_qr_code_to_clipboard()">
							<v-icon left>mdi-qrcode</v-icon>
							{{ $t('userpair_copy_qrcode') }}
						</v-btn>
						<v-btn v-if="!this.able_to_share" @click="copy_link_to_clipboard()">
							<v-icon left>mdi-web</v-icon>
							{{ $t('userpair_copy_link') }}
						</v-btn>
					</v-col>
				</v-row>
				<v-col>
					{{ $t('userpair_puplic_search_header') }}
				</v-col>
				<v-row>
					<v-col>
						<v-text-field v-model="query.searchtext" :label="$t('userpair_puplic_search_text')" clearable
							@change="userpair_query_available_publics(0)"></v-text-field>
						<v-btn icon @click="userpair_query_available_publics(0)">
							<v-icon>mdi-magnify</v-icon>
						</v-btn>
					</v-col>
				</v-row>
			</v-form>
		</v-card>
		<v-list>
			<v-list-item v-for="public_info in public_info_list" :key="public_info.uuid">
				<v-list-item-content>
					<v-list-item-title v-text="public_info.nickname"></v-list-item-title>
					<v-list-item-subtitle v-text="localDate(public_info.timestamp, $t('locale_date_time_format')) +
						' • ' +
						duration(public_info.duration)
						"></v-list-item-subtitle>
					<v-expand-transition>
						<div v-show="public_info.description_show">
							<v-divider></v-divider>

							<v-card-text>{{ public_info.description }}</v-card-text>
						</div>
					</v-expand-transition>
				</v-list-item-content>

				<v-list-item-action>
					<v-btn icon class="mx-4" @click="add_public_friendAdd(public_info.uuid)">
						<v-icon size="24px">mdi-video-plus</v-icon>
					</v-btn>
					<v-btn icon @click="public_info.description_show = !public_info.description_show">
						<v-icon>{{
							public_info.description_show
								? "mdi-chevron-up"
								: "mdi-chevron-down"
						}}</v-icon>
					</v-btn>
				</v-list-item-action>
			</v-list-item>
		</v-list>
		<v-row justify="center">
			<v-dialog dark v-model="waitSearchResults" max-width="300px">
				<!--<v-dialog  max-width="300px"> -->
				<v-card>
					<v-card-title>{{ $t("userpair_wait_for_search") }}</v-card-title>
					<v-divider></v-divider>
					<v-card-text style="height: 75px">
						<v-progress-circular indeterminate color="primary"></v-progress-circular>
					</v-card-text>
				</v-card>
			</v-dialog>
		</v-row>
		<ConfirmDlg ref="confirm" />
	</v-container>
</template>

<script>
import router from "../router";
import messenger from "../messenger";
import ConfirmDlg from "./ConfirmDlg";
// https://www.npmjs.com/package/qrcodejs2
import QRCode from "qrcodejs2";

export default {
	name: "UserPair",
	components: {
		ConfirmDlg
	},
	data() {
		return {
			uuid: 0,
			invitation: null,
			instruction: "empty",
			query: {},
			public_info_list: [],
			prev_page: -1,
			query_start_page: 0,
			next_page: -1,
		}
	},
	computed: {
		waitSearchResults: function () {
			return this.public_info_list == null;
		},
		able_to_share: function () {
			return 'share' in navigator
		},
	},
	created() {

		try {
			messenger.register("userpair", this.messenger_onMessage, null, null);
			if (localStorage.userName && localStorage.token) {
				var token = localStorage.token;
				messenger.init(token);
			} else {
				this.nav2Login();
			}
			this.uuid = this.$route.params.uuid;
			if (this.$route.params.query) {
				this.query = this.$route.params.query;
				if (this.query.name !== "") {
					this.userpair_query_available_publics(this.query_start_page);
				}
			} else {
				this.query = {
					name: "",
					source_items: [],
					source_values: [],
					provider_items: [],
					provider_values: [],
					category_items: [],
					category_values: [],
					searchtext: "",
				};
			}

		} catch (error) {
			console.log("UserPair exception", error);
			this.nav2Main();
		}

	},
	mounted() {
		this.qrcode = new QRCode("qrcode")

		this.invitation = this.$route.query.d
		if (this.invitation) {
			console.log("userpair_check_friend for invitation", this.invitation);
			messenger.emit("userpair_check_friend", {
				invitation: this.invitation
			});
		} else {
			// no invitation in url, so we generate one for the user.
			this.userpair_request_invitation()
		}
	},
	methods: {
		nav2Main() {
			router.push({ name: "Home" }); // always goes 'back enough' to Main
		},
		nav2Login() {
			router.push({ name: "Login" });
		},
		userpair_add_public_friend(uuid) {
			console.log("userpair_add_public_friend", uuid);
			messenger.emit("userpair_add_public_friend", {
				uuid: uuid,
			});
			this.nav2Main();
		},
		userpair_request_invitation() {
			console.log("userpair_request_invitation",);
			messenger.emit("userpair_request_invitation", {});
		},

		messenger_onMessage(type, data) {
			console.log("incoming message to UserPair", type, data);
			if (type == "userpair_request_invitation") {
				this.invitation = data.invitation;
				this.instruction = data.errormessage;
				this.invitation_url = window.location.protocol + "//" + window.location.host + window.location.pathname + window.location.hash + "?d=" + this.invitation;
				console.log("invitation_url", this.invitation_url, this.invitation_url.length);
				this.qrcode.clear(); // clear the code.
				this.qrcode.makeCode(this.invitation_url); // make another code.

			}
			if (type == "userpair_check_friend") {
				this.instruction = data.errormessage;
				if ("nickname" in data && "invitation" in data) {
					this.confirmPairing(data.nickname, data.invitation)
				} else {
					this.show_dialog(this.$t("error"), this.$t(data.errormessage));
				}
			}
			if (type == "userpair_pair_with_friend") {
				this.instruction = data.errormessage;
				this.show_dialog(this.$t("success"), this.$t(data.errormessage));
			}
			if (type == "userpair_query_available_publics") {
				this.public_info_list = data.public_info_list;
			}
		},
		userpair_query_available_publics(query_start_page) {
			console.log("userpair_query_available_publics");
			messenger.emit("userpair_query_available_publics", {
				select_source_values: this.query.source_values,
				select_provider_values: this.query.provider_values,
				select_category_values: this.query.category_values,
				select_searchtext: this.query.searchtext,
				query_start_page: query_start_page,
			});
			this.public_info_list = null;
		},
		async confirmPairing(nickname, invitation) {
			if (
				await this.$refs.confirm.open(
					this.$t("confirm"),
					this.$t("userpair_ok_to_pair") + " " + nickname + "?"
				)
			) {
				this.userpair_pair_with_friend(invitation);
			}
		},
		async show_dialog(title, message) {
			if (
				await this.$refs.confirm.open(
					title,
					message
				)
			) {
				this.nav2Main();
			}
		},
		userpair_pair_with_friend(invitation) {
			console.log("userpair_pair_with_friend", invitation);
			messenger.emit("userpair_pair_with_friend", {
				invitation: invitation
			});
		},
		copy_qr_code_to_clipboard() {
			var qr_code_img = document.querySelector("#qrcode > img");
			if (qr_code_img) {
				var img_src = qr_code_img.src;
				console.log("Copying QR code to clipboard from src", img_src);
				// Source - https://stackoverflow.com/q/66962885
				// Posted by Andres S, modified by community. See post 'Timeline' for change history
				// Retrieved 2026-02-28, License - CC BY-SA 4.0

				const { ClipboardItem } = window;

				fetch(img_src)
					.then(res => res.blob())
					.then(blob => {
						console.log("Blob created from QR code image", blob);
						const item = new ClipboardItem({ "image/png": blob });
						navigator.clipboard.write([item]);
						console.log(this.$t("userpair_copy_qrcode") + " " + this.$t("success"));
					})
					.catch(err => {
						console.error("Error copying QR code to clipboard:", err);
						console.log(this.$t("userpair_copy_qrcode") + " " + this.$t("error"));
					});

			} else {
				console.log(this.$t("userpair_copy_qrcode") + " " + this.$t("error"));
			}
		},
		copy_link_to_clipboard() {
			navigator.clipboard.writeText(this.invitation_url)
				.then(() => {
					console.log(this.$t("userpair_copy_link") + " " + this.$t("success"));
				})
				.catch(err => {
					console.error("Error copying link to clipboard:", err);
					console.log(this.$t("userpair_copy_link") + " " + this.$t("error"));
				});
		},
		share_invitation() {
			// working sample: https://w3c.github.io/web-share/demos/share-files.html
			// Note: Sharing files is not supported in all browsers that support the Web Share API, so we share the link and hope for the best. Sharing the QR code image would be better, but is not supported in Safari on iOS as of 2026-02-28.
			var qr_code_img = document.querySelector("#qrcode > img");
			if (qr_code_img) {
				var img_src = qr_code_img.src;
				console.log("Copying QR code to clipboard from src", img_src);
				fetch(img_src)
					.then(res => res.blob())
					.then(blob => {
						console.log("Blob created from QR code image", blob);
						var files = [new File([blob], 'qrcode.png', { type: blob.type })]
						if (!(navigator.canShare({ files }))) {
							files = null;
							for (const file of files) {
								console.log(`File info: name - ${file.name}, size ${file.size}, type ${file.type}`);
							}
							console.warn("Web Share API does not support sharing files on this browser. Sharing the invitation link without the QR code image instead.");
						}
						navigator.share({
							title: this.$t("userpair_share_title"),
							text: this.$t("userpair_share_text"),
							url: this.invitation_url,
							//files: files, // Note: 'files' is not supported in all browsers that support Web Share API

						})
							.then(() => console.log(this.$t("userpair_share_invitation") + " " + this.$t("success")))
							.catch((error) => console.error("Error sharing invitation:", error));
					})
					.catch(err => {
						console.error("Error copying QR code to blob:", err);
						console.log(this.$t("userpair_share_invitation") + " " + this.$t("error"));
					});

			} else {
				console.log(this.$t("userpair_share_invitation") + " " + this.$t("error"));
			}
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
