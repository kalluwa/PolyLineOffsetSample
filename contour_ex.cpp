#include <iostream>
#include "cavc/polylineoffset.hpp"

#include <sstream>
#include <string>
#include <fstream>
#include <canvas.h>
#include <glm/glm.hpp>
#include "glm-aabb/aabb.hpp"

//#include <fmt/core.h>
int main()
{
    
    cavc::Polyline<double> input;

    std::ifstream infile;
    infile.open("../debug_loop_pts.txt");
    std::string line;
    //get num of vertices
    if(infile.good())
    {
        getline(infile,line);
        int num_v;
        std::istringstream iss(line);
        iss >> num_v;
        while(std::getline(infile,line))
        {
            //parse
            std::istringstream iss_v(line);
            float x,y,b;
            iss_v >> x >> y >> b;

            //multiply 100 to solve precision problem
            input.addVertex(x*100.0,y*100.0,b);
        }
    }

    // compute the resulting offset polylines, offset = 3
    std::vector< std::vector<cavc::Polyline<double>>> lists;
    double step = -5.0;
    for(int i=0;i<5;i++)
    {
        if(i==0)
        {
            auto results = cavc::parallelOffset(input, step);
            lists.emplace_back(results);
        }
        else
        {
			auto results = cavc::parallelOffset(lists[lists.size()-1][0], step);
			lists.emplace_back(results);
        }
    }
    


    //draw all
	canvas::set_title(L"polyline Example");
    int window_size=600;
	canvas::set_size(window_size, window_size);

        canvas::on_render([&](){
        using namespace canvas::color;

        canvas::buffer buf;
        buf.set_size(window_size, window_size, gray);

        glm::AABB aabb;
		auto vertices = input.vertexes();
		for (int i = 0; i < vertices.size() ; i++)
        {
            auto v = vertices[i];
            aabb.extend(glm::vec3(v.x(),v.y(),0.0f));
        }
        auto min_p = aabb.getMin();
        auto max_p = aabb.getMax();
		//fmt::print("min {0} {1}\n", min_p.x,min_p.y);
		//fmt::print("max {0} {1}\n", max_p.x, max_p.y);
        auto dis_size = max_p - min_p;
		float aspect = dis_size.x / dis_size.y;
		auto offset = glm::vec3(window_size, window_size, 0) * 0.2f;
        auto scale = glm::vec3(window_size- 2*offset.x,window_size -2*offset.y,0.0f) / (dis_size);
        
        {
			auto vertices = input.vertexes();
			for(int i=0;i<vertices.size()-1;i++)
			{
				auto v_s = vertices[i];
                auto v_e = vertices[(i+1)%vertices.size()];

				auto cv_s = offset + (glm::vec3(v_s.x(), v_s.y(), 0.0) - min_p) * scale;
				auto cv_e = offset + (glm::vec3(v_e.x(), v_e.y(), 0.0) - min_p) * scale;
                buf.draw_line(cv_s.x,cv_s.y,cv_e.x,cv_e.y,yellow);
			}
        }
        for(auto lines : lists)
        {
            for(auto& line : lines)
		    {
			    auto vertices = line.vertexes();
			    for (int i = 0; i < vertices.size() - 1; i++)
			    {
				    auto v_s = vertices[i];
				    auto v_e = vertices[(i + 1) % vertices.size()];

				    auto cv_s =offset +  (glm::vec3(v_s.x(), v_s.y(), 0.0) - min_p) * scale;
				    auto cv_e =offset +  (glm::vec3(v_e.x(), v_e.y(), 0.0) - min_p) * scale;
				    buf.draw_line(cv_s.x, cv_s.y, cv_e.x, cv_e.y, red);
			    }
		    }
        }
        
        canvas::get_frame().stretch(buf);

        /*  Draw the canvas.
         */
        canvas::finalize();

        /*  Reset the render function.
         */
        canvas::done();
    });

  //  canvas::on_render([&](){
  //      using namespace canvas::color;

  //      canvas::buffer buf;
  //      buf.set_size(800, 600, gray);

  //      glm::AABB aabb;
		//for (auto& line : results)
		//{
		//	auto vertices = line.vertexes();
		//	for (int i = 0; i < vertices.size() ; i++)
  //          {
  //              auto v = vertices[i];
  //              aabb.extend(glm::vec3(v.x(),v.y(),0.0f));
  //          }
  //      }
  //      auto min_p = aabb.getMin();
  //      auto max_p = aabb.getMax();
		////fmt::print("min {0} {1}\n", min_p.x,min_p.y);
		////fmt::print("max {0} {1}\n", max_p.x, max_p.y);
  //      auto dis_size = max_p - min_p;
  //      float aspect = dis_size.x / dis_size.y;
  //      auto scale = glm::vec3(600,600,0) / (dis_size);
  //      for(auto& line : results)
  //      {
		//	auto vertices = line.vertexes();
		//	for(int i=0;i<vertices.size()-1;i++)
		//	{
		//		auto v_s = vertices[i];
  //              auto v_e = vertices[(i+1)%vertices.size()];

		//		auto cv_s = (glm::vec3(v_s.x(), v_s.y(), 0.0) - min_p) * scale;
		//		auto cv_e = (glm::vec3(v_e.x(), v_e.y(), 0.0) - min_p) * scale;
  //              buf.draw_line(cv_s.x,cv_s.y,cv_e.x,cv_e.y,yellow);
		//	}
  //      }
  //      
  //      canvas::get_frame().stretch(buf);

  //      /*  Draw the canvas.
  //       */
  //      canvas::finalize();

  //      /*  Reset the render function.
  //       */
  //      canvas::done();
  //  });

    return canvas::run();
}