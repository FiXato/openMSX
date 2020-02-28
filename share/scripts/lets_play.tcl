namespace eval lets_play {
	set debug_log 1
	proc log {args} {
		variable debug_log
		if {$debug_log != 1} { return }
		puts "DEBUG: $args"
	}

	proc position_to_time {position} {
		if {[string is double $position] == 1} {
		  log "number: $position"
			return $position
		}
		if {[dict exist [reverse status] $position] == 1} {
			log "reverse position: $position: [dict get [reverse status] $position]"
			return [dict get [reverse status] $position]
		}
		if {[dict exist $reverse::bookmarks $position] == 1} {
		  log "bookmark: [dict get $reverse::bookmarks $position]"
			return [dict get $reverse::bookmarks $position]
		}
		error "Unknown position $position"
		return $position
	}

	proc record_reverse {args} {

		variable initial_states
		variable autopause

		# Defaults
		set start_position begin
		set stop_position end
		set autopause off
		set title ""

		while (1) {
			switch -- [lindex $args 0] {
				"-from" {
					set start_position [lindex $args 1]
					set args [lrange $args 2 end]
				}
				"-to" {
					set stop_position [lindex $args 1]
					set args [lrange $args 2 end]
				}
				"-prefix" {
					set prefix [lindex $args 1]
					set args [lrange $args 2 end]
				}
				"-title" {
					set title [lindex $args 1]
					set args [lrange $args 2 end]
				}
				"-autopause" {
					set autopause on
					set args [lrange $args 1 end]
				}
				"default" {
					break
				}
			}
		}
		if {$title != ""} { set title "'$title' "}

		set prefix "openMSX - Let's Play $title\($start_position - $stop_position\) - "

		log "start position: $start_position"
		set start_time [position_to_time $start_position]
		log "start time: $start_time"
		
		set stop_time [position_to_time $stop_position]
		log "stop_time: $stop_time"

		set initial_states [dict create \
			pause $::pause \
			viewonlymode [reverse viewonlymode] \
			reverse_pos [dict get [reverse status] current] \
			scale_factor $::scale_factor \
			resampler $::resampler \
			throttle $::throttle \
		]
		
	  set ::pause on

		#FIXME: Disabling this recording speed optimisation for now as openMSX will complain the video settings were changed *while* recording, even though AFAICT I'm doing it *before*
		#set ::scale_factor 1

		set ::resampler fast
		set ::throttle off
		reverse viewonlymode true
		
		reverse goto $start_time
		
		log "Setting timer: [expr $stop_time - $start_time]"
		after time [expr $stop_time - $start_time] [namespace code stop_lets_play_recording]
		
		# Let's start the recording
		record start -triplesize -stereo -prefix "$prefix"

		set ::pause off
		return 
	}
	
	proc stop_lets_play_recording {} {
		variable initial_states
		variable autopause
		set ::pause on

		puts "Stopping Let's Play recording and returning to state before recording was started."
		record stop

		puts "reversing back to [dict get $initial_states reverse_pos]"
		reverse goto [dict get $initial_states reverse_pos]
		
		puts "setting viewonlymode back to [dict get $initial_states viewonlymode]"
		reverse viewonlymode [dict get $initial_states viewonlymode]

		puts "setting resampler back to [dict get $initial_states resampler]"
		set ::resampler [dict get $initial_states resampler]
	
	#FIXME: Disabling this recording speed optimisation for now as openMSX will complain the video settings were changed *while* recording, even though AFAICT I'm doing it *before*
	# puts "setting scale_factor back to [dict get $initial_states scale_factor]"
	#	set ::scale_factor [dict get $initial_states scale_factor]

		puts "setting throttle back to [dict get $initial_states throttle]"
		set ::throttle [dict get $initial_states throttle]

		if {$autopause != on} { 
			puts "setting pause back to [dict get $initial_states pause]"
			set ::pause [dict get $initial_states pause]
		}
		return 
	}
	

	namespace export record_reverse
	namespace export position_to_time

}

namespace import lets_play::*


# A test to see if I can get it to not complain about changing video settings while recording even though it's changed *before* recording
proc lp_test {} {
	set ::scale_factor 1
	record start -prefix -triplesize "lp-test - "
}