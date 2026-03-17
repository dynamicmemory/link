#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../include/message.hpp"
#include "../include/server.hpp"
#include "../include/client.hpp"

namespace py = pybind11;

PYBIND11_MODULE(link, m) {
    py::class_<Message>(m, "Message")
        .def_readonly("fd", &Message::fd)
        .def_readonly("payload", &Message::payload);

    py::class_<Server>(m, "Server")
        .def(py::init<const std::string&, const std::string&, const std::string&,
                      const std::string&, const std::string&>())
        .def("tick", &Server::tick)
        .def("has_message", &Server::has_message)
        .def("next", &Server::next)
        .def("send", &Server::send);

    py::class_<Client>(m, "Client")
        .def(py::init<const std::string&, const std::string&, const std::string&,
                      const std::string&, const std::string&>())
        .def("tick", &Client::tick)
        .def("has_message", &Client::has_message)
        .def("next", &Client::next)
        .def("send", &Client::send)
        .def("is_ready", &Client::is_ready);
}
