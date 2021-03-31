#include<thread>
#include<glibmm/main.h>
#include<gstreamermm.h>
#include<thread>
#include<filesystem>
#include<iostream>
#include"zgui.h"
using namespace std;

class Player : public z::AsciiWindow
{
public:
	Player()
		: z::AsciiWindow{R"(
			WMusic Player---------------------------
			|
			|    T0-------------------B4-  B5---
			|    |music file|         ||   |open|
			|
			|    S0------------------------------
			|    |0 100 1|
			|
			|    B0--- B1--- B2--- B3---
			|    |<<|  |=|   |>|   |>>|
			|)", 15, 21}
	{
		std::vector<std::string> v;
		for(const filesystem::path &p : filesystem::directory_iterator("./"))
			if(is_regular_file(p) && p.extension() == ".mp3" || p.extension() == ".wav")
				v.push_back(p.filename());
		tie("open file", 30, *T[0], *B[4], v, 700, 150);//make combobox
		S[0]->value(0);
		S[0]->draw();
		update(*S[0]);
		start(cv::WINDOW_AUTOSIZE);

		B[5]->click([this]() {
			pipe_->set_state(Gst::STATE_NULL);
			filesrc_->set_property("location", T[0]->value());
			pipe_->set_state(Gst::STATE_PAUSED);
			pipe_->query_duration(Gst::FORMAT_TIME, duration_);
			cout << "duration : " << duration_ << endl;
		});
		B[1]->click([this]() {
				pipe_->set_state(Gst::STATE_PAUSED);
				conn_.disconnect();
		});
		B[2]->click([this]() {
			pipe_->set_state(Gst::STATE_PLAYING);
			conn_ = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Player::update_pos), 200);
		});

		Gst::init();
		try {
			pipe_ = Gst::Pipeline::create();
			mainloop_ = Glib::MainLoop::create();
			decodebin_ = Gst::ElementFactory::create_element("decodebin");
			filesrc_ = Gst::ElementFactory::create_element("filesrc");
			audioconvert_ = Gst::ElementFactory::create_element("audioconvert");
			autoaudiosink_ = Gst::ElementFactory::create_element("autoaudiosink");
			pipe_->add(filesrc_)->add(decodebin_)->add(audioconvert_)->add(autoaudiosink_);
			filesrc_->link(decodebin_);
			audioconvert_->link(autoaudiosink_);
		} catch(...) {

		}


		decodebin_->signal_pad_added().connect( [this](const Glib::RefPtr<Gst::Pad> &pad) {
				pad->link(audioconvert_->get_static_pad("sink")); });
		
		pipe_->get_bus()->add_watch([this](const Glib::RefPtr<Gst::Bus>&,
					const Glib::RefPtr<Gst::Message> &message)
		{// Print type of the message posted on the bus, and the source object name.
			switch(message->get_message_type()) {
			case Gst::MESSAGE_ERROR: {// Handle ERROR message - print error and debug information
				auto error_msg = Glib::RefPtr<Gst::MessageError>::cast_static(message);
				cout << "Error: " << error_msg->parse_error().what() << endl;
				cout << "Debug: " << error_msg->parse_debug() << endl;
				break;
			}
			case Gst::MESSAGE_EOS: // Handle EOS message - quit the loop
				mainloop_->quit();
				break;
			case Gst::MESSAGE_STATE_CHANGED:
				break;
			case Gst::MESSAGE_PROGRESS:
				update_pos();
			default: break; // Unhanlded messages
			}
			return true;
		});
	}
protected:
	Glib::RefPtr<Gst::Pipeline> pipe_;
	Glib::RefPtr<Glib::MainLoop> mainloop_;
	Glib::RefPtr<Gst::Element> decodebin_, autoaudiosink_, filesrc_, audioconvert_;
	sigc::connection conn_;
	gint64 duration_;
private:
	bool update_pos() {
		gint64 pos, dur;
		if(pipe_->query_position(Gst::FORMAT_TIME, pos)
				&& pipe_->query_duration(Gst::FORMAT_TIME, dur)) {
			//cout << "current position : " << pos << endl;
			S[0]->value(pos * 100 / dur);
			*this << *S[0];
		}
		return true;
	}
};


int main()
{
	Player win;
	return win.loop();
}
