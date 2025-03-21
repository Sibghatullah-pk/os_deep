#!/bin/bash

# Display name and roll number
echo "23—0896-Sibghatullah"

# Function to create a user and assign admin privileges
create_user() {
    # Take user input for username and password
    read -p "Enter user: " USERNAME
    read -sp "Enter password: " PASSWORD
    echo  # New line after password input

    # Check if user already exists
    if id "$USERNAME" &>/dev/null; then
        echo "User $USERNAME already exists!"
        return 1
    fi

    # Create user with home directory and set password
    sudo useradd -m -s /bin/bash "$USERNAME"
    echo "$USERNAME:$PASSWORD" | sudo chpasswd

    # Add user to sudo group for admin privileges
    sudo usermod -aG sudo "$USERNAME"

    # Verify user creation
    if id "$USERNAME" &>/dev/null; then
        echo "User $USERNAME is created and administrator privileges are assigned"
    else
        echo "Failed to create user $USERNAME!"
        exit 1
    fi
}

# Function to list installed applications
list_applications() {
    echo "Listing all installed applications..."
    dpkg -l | grep '^ii' | awk '{print $2}' > installed_apps.txt
    echo "Installed applications saved to installed_apps.txt"
    head -n 5 installed_apps.txt  # Show first 5 for verification
}

# Function to install Dropbox
install_dropbox() {
    echo "Installing Dropbox..."

    # Download Dropbox installer
    wget -O dropbox.deb "https://www.dropbox.com/download?plat=lnx.x86_64"

    # Install Dropbox
    sudo dpkg -i dropbox.deb
    sudo apt-get install -f -y  # Fix any dependency issues

    # Verify installation
    if command -v dropbox &>/dev/null; then
        echo "Dropbox installed successfully!"
    else
        echo "Failed to install Dropbox!"
        exit 1
    fi
}

# Function to configure IP, mask, gateway, and DNS
configure_network() {
    IP="10.0.0.1"
    MASK="255.255.255.0"
    GATEWAY="10.0.0.254"
    DNS="8.8.8.8"
    INTERFACE=$(ip -o link show | awk -F': ' '{print $2}' | grep -v lo | head -n 1)

    echo "Configuring network settings on interface $INTERFACE..."

    # Set IP address and mask
    sudo ip addr flush dev "$INTERFACE"
    sudo ip addr add "$IP/$MASK" dev "$INTERFACE"
    sudo ip link set "$INTERFACE" up

    # Set gateway
    sudo ip route add default via "$GATEWAY"

    # Set DNS
    echo "nameserver $DNS" | sudo tee /etc/resolv.conf

    # Verify network configuration
    echo "Verifying network settings..."
    ip addr show "$INTERFACE"
    ip route show
    cat /etc/resolv.conf

    # Test connectivity
    if ping -c 4 8.8.8.8 &>/dev/null; then
        echo "Network configuration successful! Internet is reachable."
    else
        echo "Network configuration failed! Cannot reach the internet."
        exit 1
    fi
}

# Function to display help
show_help() {
    echo "Usage: $0 [SWITCH]"
    echo "Switches:"
    echo "  --uc     Create user with admin privileges (requires user input)"
    echo "  --ld     List all installed applications"
    echo "  --ins    Install Dropbox"
    echo "  --ipcon  Configure IP, mask, gateway, and DNS"
    echo "  --help   Show this help message"
    echo "  (No switch) Run all tasks"
}

# Function to run all tasks
run_all_tasks() {
    echo "Running all tasks..."
    create_user
    list_applications
    install_dropbox
    configure_network
}

# Main script logic
if [ $# -eq 0 ]; then
    # If no switch is provided, run all tasks
    run_all_tasks
else
    case "$1" in
        --uc)
            create_user
            ;;
        --ld)
            list_applications
            ;;
        --ins)
            install_dropbox
            ;;
        --ipcon)
            configure_network
            ;;
        --help)
            show_help
            ;;
        *)
            echo "Invalid switch! Use --help to see available options."
            exit 1
            ;;
    esac
fi
