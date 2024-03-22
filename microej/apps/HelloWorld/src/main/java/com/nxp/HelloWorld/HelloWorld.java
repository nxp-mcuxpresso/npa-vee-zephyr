/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
package com.nxp.HelloWorld;
import ej.util.Device;

public class HelloWorld {

	private static HelloWorld helloWorld;

	/**
	 * Public constructor
	 */
	public HelloWorld() {
        String id = new String(Device.getId());

        while (true) {
            System.err.println("Platform Accelerator VM running on NXP " + id);
            System.err.println("Hello World!\n");
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
	}

	/**
	 * Simple main.
	 *
	 * @param args
	 *            command line arguments.
	 */
	public static void main(String[] args) {
		helloWorld = new HelloWorld();
	}
}
