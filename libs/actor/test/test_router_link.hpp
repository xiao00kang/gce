﻿///
/// Copyright (c) 2009-2014 Nous Xiong (348944179 at qq dot com)
///
/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///
/// See https://github.com/nousxiong/gce for latest version.
///

namespace gce
{
class router_link_ut
{
public:
  static void run()
  {
    std::cout << "router_link_ut begin." << std::endl;
    test_base();
    std::cout << "router_link_ut end." << std::endl;
  }

public:
  static void test_base()
  {
    try
    {
      std::size_t quiter_num = 100;

      attributes attrs;
      attrs.id_ = atom("router");
      context ctx(attrs);
      attrs.id_ = atom("one");
      context ctx1(attrs);
      attrs.id_ = atom("two");
      context ctx2(attrs);
      
      actor<threaded> base = spawn(ctx);
      actor<threaded> base1 = spawn(ctx1);
      actor<threaded> base2 = spawn(ctx2);

      gce::bind(base, "tcp://127.0.0.1:14923", true);

      net_option opt;
      opt.reconn_period_ = seconds_t(1);
      connect(base1, atom("router"), "tcp://127.0.0.1:14923", true, opt);
      connect(base2, atom("router"), "tcp://127.0.0.1:14923", true, opt);
      wait(base2, boost::chrono::milliseconds(100));

      std::vector<aid_t> quiter_list(quiter_num);
      for (std::size_t i=0; i<quiter_num; ++i)
      {
        quiter_list[i] =
          spawn(
            base2,
            boost::bind(
              &router_link_ut::quiter, _1
              ),
            monitored
            );
        base1.link(quiter_list[i]);
      }

      for (std::size_t i=0; i<quiter_num; ++i)
      {
        send(base1, quiter_list[i]);
      }

      for (std::size_t i=0; i<quiter_num; ++i)
      {
        recv(base1);
        recv(base2);
      }
    }
    catch (std::exception& ex)
    {
      std::cerr << "test_base except: " << ex.what() << std::endl;
    }
  }

  static void quiter(actor<stackful>& self)
  {
    try
    {
      recv(self);
    }
    catch (std::exception& ex)
    {
      std::cerr << "quiter except: " << ex.what() << std::endl;
    }
  }
};
}

