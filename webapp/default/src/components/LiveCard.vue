<!-- https://blog.kulturbanause.de/2013/12/css-grid-layout-module/ -->
<template>
  <v-card class="mx-auto,Frienzz-livecard" color="friend_info.public==true ? 'blue' : 'lightgreen' ">
    <v-card-title>
      <v-icon v-if="friend_info.available_time > 0" size="200%" left
        :color="get_icon_for_available_type(friend_info.available_type, true)">
        {{ get_icon_for_available_type(friend_info.available_type, false) }}
      </v-icon>
      <span class="text-h5 font-weight-bold">{{ friend_info.friend_nickname }}</span>
    </v-card-title>

    <v-card-text class="text-body font-weight-light">
      {{ friend_info.info }}
    </v-card-text>
    <v-card-actions>
      <v-list-item>

        <span class="mr-1">&nbsp;</span>
        <!-- the live movies have a special duration value, which tells how many percent of the live movie has already passed -->
        <!--span align="left" id="viewed" :style="{ width: get_slider_percentage() }"></span-->
        <v-progress-linear v-show="friend_info.available_time > 0"
          :color="get_icon_for_available_type(friend_info.available_type, true)" rounded height="25"
          :value="get_slider_percentage()">({{ duration(friend_info.available_time) }})</v-progress-linear>
        <span v-if="friend_info.available_time > 0">
          &#x1F551; {{
            localDateTime(
              friend_info.available_time + Math.floor((new Date()).getTime() / 1000),
              $t("locale_time_format")
            )
          }}
        </span>
        <v-spacer></v-spacer>
        <v-row align="center" justify="end">
          <!--
          <card-menu :friend_info="friend_info" />
          -->
          <v-btn icon @click="nav2Edit(
            {
              available_icons: get_icons_for_available_types(),
              get_icon_for_available_type: get_icon_for_available_type,
              friend_info: friend_info

            })">
            <v-icon color="blue darken-1">mdi-pencil</v-icon>
          </v-btn>
          <v-btn icon @click="description_show = !description_show">
            <v-icon color="orange darken-1">{{ description_show ? 'mdi-chevron-up' : 'mdi-chevron-down' }}</v-icon>
          </v-btn>
        </v-row>
      </v-list-item>
    </v-card-actions>
    <v-expand-transition>
      <div id="description" v-show="description_show">
        {{ friend_info.description }}
      </div>
    </v-expand-transition>
  </v-card>
</template>


<script>
//import CardMenu from "./CardMenu.vue"
export default {
  name: "livecard",
  components: {
    //CardMenu
  },
  props: {
    friend_info: Object,
    max_seconds: Number
  },
  data() {
    return {
      description_show: false
    }
  },
  inject: ['nav2Edit',
    'nav2Play',
    'localDateTime',
    'duration',
    'get_icon_for_available_type',
    "get_icons_for_available_types"
  ],
  created() {
  },
  methods: {
    get_slider_percentage() {
      return Math.floor((100 * this.friend_info.available_time / this.max_seconds))
    }
  }

};
</script>

<style scoped>
.Frienzz-livecard_old {
  display: grid;
  grid-template-columns: 10px 1fr 1fr 1fr 1fr 40px;
  grid-template-rows: repeat(5, min-content);
  gap: 0px 0px;
  grid-template-areas:
    "marker nickname nickname nickname nickname nickname"
    "marker info info info info info"
    "marker duration time . . edit"
    "marker viewed viewed viewed  viewed show"
    "marker description description description description description"
  ;
  background: rgb(128, 128, 128);
  border-radius: 10px;
  padding: 5px;
  margin-bottom: 10px;
}

#marker {
  background: rgb(255, 0, 0);
  grid-area: marker;
  border-radius: 5px;
}

#nickname {
  background: grey;
  color: white;
  grid-area: nickname;
  font-family: "Atkinson-Hyperlegible", Helvetica, Arial;
  font-size: 200%;
  line-height: 100%;
  text-align: left;
  padding-left: 5px;
  font-weight: bold;

}

#info {
  background: grey;
  color: white;
  grid-area: info;
  font-family: "Atkinson-Hyperlegible", Helvetica, Arial;
  font-size: 150%;
  text-align: left;
  padding-left: 5px;
}

#time {
  background: grey;
  color: lightgray;
  font-weight: bold;
  grid-area: time;
}

#duration {
  background: grey;
  color: lightgray;
  font-weight: bold;
  grid-area: duration;
}

#viewed {
  background: orange;
  /*border-radius: 13px;*/
  /* (height of inner div) / 2 + padding */
  padding: 3px;
  width: 10%;
  height: 20px;
  border-radius: 10px;
}



#edit {
  background: grey;
  color: white;
  grid-area: edit;
}

#record {
  background: grey;
  color: white;
  grid-area: record;
}

#show {
  background: grey;
  color: white;
  grid-area: show;
}

#description {
  background: grey;
  color: white;
  grid-area: description;
}
</style>