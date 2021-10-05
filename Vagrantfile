# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  
  # these plugins will be automativally installed
  config.vagrant.plugins = ["vagrant-vbguest", "vagrant-proxyconf"]

  # Configure the ports for webx
  config.vm.network "forwarded_port", guest: 5555, host: 5555, host_ip: "127.0.0.1"
  config.vm.network "forwarded_port", guest: 5556, host: 5556, host_ip: "127.0.0.1"
  config.vm.network "forwarded_port", guest: 5557, host: 5557, host_ip: "127.0.0.1"

  config.ssh.forward_agent = true

  config.vm.provider "virtualbox" do |virtual_box|

    virtual_box.gui = true
    virtual_box.memory = "2048"
    virtual_box.cpus = 4
  
    # note: VBoxSVGA seems to be the best controller for graphics performance inside virtual box
    virtual_box.customize ['modifyvm', :id, '--graphicscontroller', 'VBoxSVGA']
    virtual_box.customize ['modifyvm', :id, '--accelerate3d', 'on']
    virtual_box.customize ['modifyvm', :id, '--vram', '128']
    virtual_box.customize ['modifyvm', :id, '--vrde', 'off']
    
    # because why not...
    virtual_box.customize ['modifyvm', :id, '--audiocontroller', 'hda']

    virtual_box.customize ["modifyvm", :id, "--clipboard", "bidirectional"]
    virtual_box.customize ["modifyvm", :id, "--draganddrop", "hosttoguest"]

  end

  config.vm.provision :shell, privileged: true, name: "Updating dependency sources", inline: "apt update"
  
  config.vm.provision :shell, privileged: true, name: "Upgrading all dependencies", inline: "apt upgrade -y"
  
  config.vm.provision :shell, privileged: true, name: "Installing desktop environment", inline: "apt install -y xfce4",  env: {"DEBIAN_FRONTEND" => "noninteractive"}
  
  config.vm.provision :shell, privileged: true, name: "Installing virtual box guest additions dependencies", inline: "apt install -y virtualbox-guest-dkms virtualbox-guest-x11"  

  config.vm.provision :shell, privileged: true, name: "Installing some useful software for tests", inline: "apt install -y chromium-browser firefox git terminator mesa-utils"

  config.vm.provision :shell, privileged: true, name: "Permitting anyone to start an X11 session", inline: "sed -i 's/allowed_users=.*$/allowed_users=anybody/' /etc/X11/Xwrapper.config"

  config.vm.provision :shell, privileged: true, name: "Installing webx server dependencies", inline: "apt install -y cmake pkg-config gdb valgrind build-essential libzmq3-dev libpng-dev libwebp-dev libjpeg-dev libxdamage-dev libxrender-dev libxext-dev libxfixes-dev libxtst-dev"

  # define our two boxes for the bionic and focal ubuntu distributions (everything else should be generic)
  config.vm.define :bionic, autostart: false do |bionic|
    bionic.vm.box = "bento/ubuntu-18.04"
    bionic.vm.hostname = "webx-bionic"
    bionic.vm.provider "virtualbox" do |bionic_virtual_box|
      bionic_virtual_box.name = "webx-bionic-box"
    end
    # give the box a static ip address
    bionic.vm.network "private_network", ip: "10.50.0.121"
  end

  config.vm.define :focal, autostart: false do |focal|
    focal.vm.box = "bento/ubuntu-20.04"
    focal.vm.hostname = "webx-focal"
    focal.vm.provider "virtualbox" do |focal_virtual_box|
      focal_virtual_box.name = "webx-focal-box"
    end
    # give the box a static ip address
    focal.vm.network "private_network", ip: "10.50.0.122"
  end

  # apply proxy settings to all boxes if the variables are defined in the host
  if ENV["http_proxy"]
    config.proxy.http = ENV["http_proxy"]
  end
  if ENV["https_proxy"]
    config.proxy.https = ENV["https_proxy"]
  end
  if ENV["no_proxy"]
    config.proxy.no_proxy = ENV["no_proxy"]
  end

end
